#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/fcntl.h>
#include <sys/stat.h>

#ifdef  RT_USING_COMPONENTS_INIT
#include <components.h>
#endif  /* RT_USING_COMPONENTS_INIT */
#include "cJSON.h"
#include "cap.h"
//#define SUB_PROCESS "[CapProcess]"
struct rt_semaphore cap_rx_sem,wifi_rx_sem,server_sem,lcd_rx_sem;
rt_device_t dev_cap,dev_wifi,dev_lcd;
int data_co2=0,data_ch2o;
char *post_message=NULL,can_send=0;
char server_time[13];
char wifi_result[512]={0};
char *http_parse_result(const char*lpbuf);
#define SRAM_MAPPING_ADDRESS 0x10000000
struct rt_memheap system_heap;

void sram_init(void)
{
    /* initialize the built-in SRAM as a memory heap */
    rt_memheap_init(&system_heap,
                    "system",
                    (void *)SRAM_MAPPING_ADDRESS,
                    1024*64);
}

void *sram_malloc(unsigned long size)
{
    return rt_memheap_alloc(&system_heap, size);
}
RTM_EXPORT(sram_malloc);

void sram_free(void *ptr)
{
    rt_memheap_free(ptr);
}
RTM_EXPORT(sram_free);

void *sram_realloc(void *ptr, unsigned long size)
{
    return rt_memheap_realloc(&system_heap, ptr, size);
}
RTM_EXPORT(sram_realloc);

unsigned int CRC_check(char *Data,char Data_length)
{
	unsigned int mid=0;
	unsigned char times=0,Data_index=0;
	unsigned int LOCAL_CRC=0xFFFF;
	while(Data_length)
	{
		LOCAL_CRC=Data[Data_index]^LOCAL_CRC;
		for(times=0;times<8;times++)
		{
			mid=LOCAL_CRC;
			LOCAL_CRC=LOCAL_CRC>>1;
			if(mid & 0x0001)
			{
				LOCAL_CRC=LOCAL_CRC^0xA001;
			}
		}
		Data_index++;
		Data_length--;
	}
	return LOCAL_CRC;
}
char *http_parse_result(const char*lpbuf)  
{
	char *ptmp = RT_NULL;      
	char *response = RT_NULL;   
	ptmp = (char*)strstr(lpbuf,"HTTP/1.1");  
	if(!ptmp)
	{
		rt_kprintf("http/1.1 not find\n");  
		return RT_NULL;
	}
	if(atoi(ptmp + 9)!=200)
	{
		rt_kprintf("result:\n%s\n",lpbuf);   
		return RT_NULL; 
	}
	ptmp = (char*)strstr(lpbuf,"\r\n\r\n"); 
	if(!ptmp)
	{
		rt_kprintf("ptmp is NULL\n");
		return RT_NULL;  
	}
	response = (char *)sram_malloc(rt_strlen(ptmp)+1);  
	if(!response)
	{
		rt_kprintf("malloc failed %d\n",rt_strlen(ptmp)+1);   
		return RT_NULL;  
	}
	strcpy(response,ptmp+4); 
	return response;
}  

char *add_item(char *old,char *id,char *text)
{
	cJSON *root;
	char *out;
	if(old!=RT_NULL)
		root=cJSON_Parse(old);
	else
		root=cJSON_CreateObject();	
	cJSON_AddItemToObject(root, id, cJSON_CreateString(text));
	//out=cJSON_Print(root);	
	out=cJSON_PrintUnformatted(root);	
	cJSON_Delete(root);
	if(old)
		sram_free(old);
	
	return out;
}
char *add_obj(char *old,char *id,char *pad)
{
	cJSON *root,*fmt;
	char *out;
	root=cJSON_Parse(old);
	fmt=cJSON_Parse(pad);
	cJSON_AddItemToObject(root, id, fmt);
	out=cJSON_Print(root);
	cJSON_Delete(root);
	cJSON_Delete(fmt);
	sram_free(pad);
	return out;
}
void send_web_get(char *buf,int timeout)
{
	char httpd_send[64]={0};//"AT+HTTPDT\r\n";
	char httpd_local[64]={0};//"AT+HTTPPH=/mango/checkDataYes\r\n";	
	char httpd_mode[64]={0};//"AT+HTTPTP=GET\r\n";	
	strcpy(httpd_mode,"AT+HTTPTP=GET\n");
	strcpy(httpd_local,"AT+HTTPPH=/saveData/airmessage/messMgr.do?JSONStr=");
	strcpy(httpd_send,"AT+HTTPDT\n");
	rt_device_write(dev_wifi, 0, (void *)httpd_mode, rt_strlen(httpd_mode));
	rt_thread_delay(30);
	char *send=(char *)sram_malloc(rt_strlen(buf)+rt_strlen(httpd_local)+1+1);
	rt_memset(send,'\0',rt_strlen(buf)+rt_strlen(httpd_local)+1+1);
	strcpy(send,httpd_local);
	strcat(send,buf);
	strcat(send,"\n");
	rt_kprintf("send %s",send);
	rt_device_write(dev_wifi, 0, (void *)send, rt_strlen(send));
	rt_thread_delay(10);
	rt_device_write(dev_wifi, 0, (void *)httpd_send, rt_strlen(httpd_send));
	sram_free(send);
	rt_sem_take(&(server_sem), timeout);//wait for server respond
}
void send_web_post(char *buf,int timeout)
{
	char httpd_send[64]={0};//"AT+HTTPDT\r\n";
	char httpd_local[64]={0};//"AT+HTTPPH=/mango/checkDataYes\r\n";
	char httpd_mode[64]={0};//"AT+HTTPTP=GET\r\n";	
	strcpy(httpd_mode,"AT+HTTPTP=POST\n");
	strcpy(httpd_local,"AT+HTTPPH=/saveData/airmessage/messMgr.do\n");
	strcpy(httpd_send,"AT+HTTPDT=JSONStr=");
	rt_kprintf("buf %s\n",buf);
	rt_device_write(dev_wifi, 0, (void *)httpd_mode, rt_strlen(httpd_mode));
	rt_thread_delay(30);
	char *send=(char *)sram_malloc(rt_strlen(buf)+rt_strlen(httpd_send)+1+1);
	rt_memset(send,'\0',rt_strlen(buf)+rt_strlen(httpd_send)+1+1);
	strcpy(send,httpd_send);
	strcat(send,buf);
	strcat(send,"\n");
	rt_kprintf("send %s",send);
	rt_device_write(dev_wifi, 0, (void *)httpd_local, rt_strlen(send));
	rt_thread_delay(10);
	rt_device_write(dev_wifi, 0, (void *)send, rt_strlen(httpd_send));
	sram_free(send);
	rt_sem_take(&(server_sem), timeout);//wait for server respond
}

char *doit_data(char *text,const char *item_str)
{	
	char *out=RT_NULL;
	cJSON *item_json;	
	item_json=cJSON_Parse(text);	
	if (!item_json)
	{
		rt_kprintf("Error before: [%s]\n",cJSON_GetErrorPtr());
	}
	else	
	{	
		if (item_json)
		{	 		
			cJSON *data;	
			data=cJSON_GetObjectItem(item_json,item_str);
			if(data)		
			{			
				int nLen = rt_strlen(data->valuestring);
				//rt_kprintf("%s ,%d %s\n",item_str,nLen,data->valuestring);			
				out=(char *)sram_malloc(nLen+1);		
				rt_memset(out,'\0',nLen+1);	
				rt_memcpy(out,data->valuestring,nLen);	
			}		
			else		
				rt_kprintf("can not find %s\n",item_str);	
		} 
		else	
			rt_kprintf("get %s failed\n",item_str); 
			cJSON_Delete(item_json);	
	}	
	return out;
}
void save_to_file(char *date,char *message)
{
	//FILE *fp;
	int fp;
	char *file_path;
	char *data;
	file_path=(char *)sram_malloc(256);
	data=(char *)sram_malloc(512);
	rt_memset(file_path,0,256);
	rt_memset(data,0,512);
	strcpy(file_path,FILE_PATH);
	rt_memcpy(file_path+rt_strlen(FILE_PATH),date,10);
	strcat(file_path,".dat");
	fp = open(file_path, O_RDONLY,0);
	if (fp < 0)
	{
		fp=open(file_path,O_WRONLY | O_CREAT ,0);
		if(fp<0)
		{
			rt_kprintf("can not create %s\r\n",file_path);
			sram_free(file_path);
			sram_free(data);
			return;
		}	
	}
	else
	{
		close(fp);
		fp=open(file_path, O_WRONLY | O_APPEND,0);
	}
	strcpy(data,date+11);
	strcat(data,"\n");
	write(fp,data,rt_strlen(data));
	rt_memset(data,'\0',512);
	strcpy(data,message);
	strcat(data,"\n");
	write(fp,data,rt_strlen(data));
	close(fp);
	sram_free(file_path);
	sram_free(data);
}

void resend_history(char *date_begin,char *date_end)
{
	FILE *fp;
	int month_b,year_b,day_b,month_e,year_e,day_e,hour_e,minute_e,max_day;
	char year_begin[5]={0};
	char year_end[5]={0};
	char month_begin[3]={0};
	char month_end[3]={0};
	char day_begin[3]={0};
	char day_end[3]={0};
	char hour_end[3]={0};
	char minute_end[3]={0};
	char file_path[256]={0};
//	char data[512]={0};
	char date[32]={0};
	rt_memcpy(year_begin,date_begin,4);
	rt_memcpy(year_end,date_end,4);
	rt_memcpy(month_begin,date_begin+5,2);
	rt_memcpy(month_end,date_end+5,2);
	rt_memcpy(day_begin,date_begin+8,2);
	rt_memcpy(day_end,date_end+8,2);
	rt_memcpy(hour_end,date_end+11,2);
	rt_memcpy(minute_end,date_end+14,2);
	month_b=atoi(month_begin);
	year_b=atoi(year_begin);
	day_b=atoi(day_begin);
	month_e=atoi(month_end);
	year_e=atoi(year_end);
	day_e=atoi(day_end);
	hour_e=atoi(hour_end);
	minute_e=atoi(minute_end);
	rt_kprintf(MAIN_PROCESS"year_b %04d,month_b %02d,day_b %02d,year_e %04d,month_e %02d,day_e %02d\r\n",year_b,month_b,day_b,year_e,month_e,day_e);
	while(1)
	{
		if(year_b<year_e || month_b<month_e || day_b<=day_e)
		{
			rt_memset(file_path,'\0',256);
			rt_memset(date,'\0',32);
			rt_sprintf(date,"%04d-%02d-%02d",year_b,month_b,day_b);
			strcpy(file_path,FILE_PATH);
			rt_memcpy(file_path+rt_strlen(FILE_PATH),date,10);
			strcat(file_path,".dat");
			rt_kprintf(MAIN_PROCESS"to open %s\r\n",file_path);
			fp = fopen(file_path, "r");
			if (fp != NULL)
			{
				int read=0,tmp_i=0;
				char * line = NULL;
				size_t len = 0;
				rt_kprintf(MAIN_PROCESS"open file %s ok\r\n",file_path);
				while ((read = getline(&line, &len, fp)) != -1) 
				{				
					if(year_b==year_e && month_b==month_e && day_b==day_e)
					{//check time in file
						if((tmp_i%2)==0)
						{							
							char local_hour[3]={0},local_minute[3]={0};
							rt_memcpy(local_hour,line,2);
							rt_memcpy(local_minute,line+3,2);
							if((atoi(local_hour)*60+atoi(local_minute))>(hour_e*60+minute_e))
							{
								rt_kprintf(MAIN_PROCESS"file_time %02d:%02d,end time %02d:%02d",atoi(local_hour),atoi(local_minute),hour_e,minute_e);
								sram_free(line);
								fclose(fp);
								return;
							}
						}
						else
						{
							line[rt_strlen(line)-1]='\0';							
							rt_kprintf(MAIN_PROCESS"rsend web %s",line);
							send_web_post(line,RT_WAITING_FOREVER);
							char *rcv=wifi_result;
							if(rt_strlen(rcv)!=0)
							{	
								int len1=rt_strlen(rcv);
								rt_kprintf(MAIN_PROCESS"<=== %s %d\n",rcv,len1);
								rt_kprintf(MAIN_PROCESS"send ok\n");
								//sram_free(rcv);
							}
						}
					}
					else
					{
						if((tmp_i%2)!=0)
						{						
							line[rt_strlen(line)-1]='\0';
							rt_kprintf(MAIN_PROCESS"rsend web %s",line);
							send_web_post(line,RT_WAITING_FOREVER);
							char *rcv=wifi_result;
							if(rt_strlen(rcv)!=0)
							{	
								int len1=rt_strlen(rcv);
								rt_kprintf(MAIN_PROCESS"<=== %s %d\n",rcv,len1);
								rt_kprintf(MAIN_PROCESS"send ok\n");
								//sram_free(rcv);
							}
						}
					}
					tmp_i++;
				}
				sram_free(line);
					
			}
			else
			{
				rt_kprintf(MAIN_PROCESS"can not open %s\r\n",file_path);
				//break;
			}
			if(month_b==2)
					max_day=28;
				else if(month_b==1||month_b==3||month_b==5||month_b==7||month_b==8||month_b==10||month_b==12)
					max_day=31;
				else
					max_day=30;
				if(day_b==max_day)
				{
					if(month_b==12)
					{
						year_b++;
						month_b=0;
					}
					else
						month_b++;
					day_b=0;
				}
				else
					day_b++;			
		}
		else
		{
			rt_kprintf(MAIN_PROCESS"end year_b %04d,month_b %02d,day_b %02d,year_e %04d,month_e %02d,day_e %02d\r\n",year_b,month_b,day_b,year_e,month_e,day_e);
			break;
		}
	}
	if(fp!=NULL)
	fclose(fp);
}

void sync_server(int fd,int resend)
{
	int i,j;
	//char text_out[512]={0};
	char *sync_message=NULL,*rcv=NULL;
	if(resend)
		sync_message=add_item(NULL,ID_DGRAM_TYPE,TYPE_DGRAM_ASK_RE_DATA);
	else
		sync_message=add_item(NULL,ID_DGRAM_TYPE,TYPE_DGRAM_SYNC);
	sync_message=add_item(sync_message,ID_DEVICE_UID,"230FFEE9981283737D");
	sync_message=add_item(sync_message,ID_DEVICE_IP_ADDR,"16.168.1.23");
	sync_message=add_item(sync_message,ID_DEVICE_PORT,"9517");
	rt_kprintf(SUB_PROCESS"<sync GET>%s\n",sync_message);
	#if 0
	j=0;
	for(i=0;i<rt_strlen(sync_message);i++)
	{
		if(sync_message[i]=='\n'||sync_message[i]=='\r'||sync_message[i]=='\t')
			j++;
	}
	char *out1=sram_malloc(rt_strlen(sync_message)-j+1);
	rt_memset(out1,'\0',rt_strlen(sync_message)-j+1);
	j=0;
	for(i=0;i<rt_strlen(sync_message);i++)
	{
		if(sync_message[i]!='\r'&&sync_message[i]!='\n'&&sync_message[i]!='\t')		
		{
			out1[j++]=sync_message[i];
		}
	}
	#endif
	send_web_post(sync_message,RT_WAITING_FOREVER);
	rcv=wifi_result;
	sram_free(sync_message);
	//sram_free(out1);
	if(rt_strlen(rcv)!=0)
	{	
		int len=rt_strlen(rcv);
		rt_kprintf(MAIN_PROCESS"<=== %s %d\n",rcv,len);
		rt_kprintf(MAIN_PROCESS"send ok\n");
		char *starttime=NULL;
		char *tmp=NULL;
		if(resend)
		{
			
			starttime=doit_data(rcv+3,(char *)"101");
			tmp=doit_data(rcv+3,(char *)"102");
			if(starttime!=NULL && tmp!=NULL)
			{
				rt_kprintf(MAIN_PROCESS"%s\r\n",tmp);
				rt_kprintf(MAIN_PROCESS"%s\r\n",starttime);
				resend_history(starttime,tmp);
				sram_free(starttime);
				sram_free(tmp);
			}
		}
		else
		{
			//strcpy(rcv,"{\"30\":\"230FFEE9981283737D\",\"210\":\"2015-08-27 14:43:57.0\",\"211\":\"???,????,???,313131\",\"212\":\"??\",\"213\":\"??\",\"104\":\"2015-09-18 11:53:58\",\"201\":[],\"202\":[]}");
			//if(atoi(type)==5)
			//{
				char year[3]={0},month[3]={0},day[3]={0},hour[3]={0},minute[3]={0},second[3]={0};
				unsigned int crc=0;
				starttime=doit_data(rcv+4,(char *)"104");
				server_time[0]=0x6c;server_time[1]=ARM_TO_CAP;
				server_time[2]=0x00;server_time[3]=0x01;server_time[4]=0x06;
				rt_memcpy(year,starttime+2,2);
				rt_memcpy(month,starttime+5,2);
				rt_memcpy(day,starttime+8,2);
				rt_memcpy(hour,starttime+11,2);
				rt_memcpy(minute,starttime+14,2);
				rt_memcpy(second,starttime+17,2);
				server_time[5]=atoi(year);server_time[6]=atoi(month);
				server_time[7]=atoi(day);server_time[8]=atoi(hour);
				server_time[9]=atoi(minute);server_time[10]=atoi(second);
				crc=CRC_check(server_time,11);
				server_time[11]=(crc&0xff00)>>8;server_time[12]=crc&0x00ff;
				write(fd,server_time,13);
				rt_kprintf(MAIN_PROCESS"SERVER TIME %s\r\n",starttime);
				//tmp=doit_data(rcv+4,(char *)"211");
				rt_kprintf(MAIN_PROCESS"211 %s\r\n",doit_data(rcv+4,"211"));
				rt_kprintf(MAIN_PROCESS"212 %s\r\n",doit_data(rcv+4,"212"));
			//}
			//else if(atoi(type)==6)
			//{
			//}
		}
		//sram_free(rcv);
	}
	return ;
}

/*get data from lv's cap board, and send to server ,save to local*/
static rt_err_t cap_rx_ind(rt_device_t dev, rt_size_t size)
{
	rt_sem_release(&(cap_rx_sem));    
	return RT_EOK;
}
void cap_thread(void* parameter)
{		
	#define STATE_IDLE 	0
	#define STATE_6C 	1
	#define STATE_AA 	2
	#define STATE_MESSAGE_TYPE 3
	#define STATE_MESSAGE_LEN 4
	#define STATE_MESSAGE 5
	#define STATE_CRC 6
	char ch,state=STATE_IDLE,message_len=0;	
	char id[32]={0},data[32]={0},date[32]={0},error[32]={0};
	char message[10],to_check[20];
	unsigned char i=0;
	int crc=0,j,message_type=0;	
	//char time[]={0x6C,0xBB,0x00,0x01,0x06,0x0F,0x09,0x10,0x09,0x09,0x09,0xE1,0xD8};
	//char httpd_send[64]={0};//"AT+HTTPDT\r\n";
	//char httpd_local[64]={0};//"AT+HTTPPH=/mango/checkDataYes\r\n";
	//strcpy(httpd_local,"AT+HTTPPH=/saveData/airmessage/messMgr.do?JSONStr=");
	//strcpy(httpd_send,"AT+HTTPDT\n");	
	//rt_device_write(dev_cap, 0, (void *)time, 13);
	while(1)	
	{		
		if (rt_sem_take(&(cap_rx_sem), RT_WAITING_FOREVER) != RT_EOK) continue;
		if(rt_device_read(dev_cap,0,&ch,1)==1)
		{
			switch (state)
			{
				case STATE_IDLE:
				{
					if(ch==0x6c)
						state=STATE_6C;
					else
						state=STATE_IDLE;
				}
				break;
				case STATE_6C:
				{
					if(ch==0xaa)
					{
						state=STATE_AA;
						i=0;
					}
					else
						state=STATE_IDLE;
				}
				break;
				case STATE_AA:
				{
					message_type=ch<<8;
					rt_kprintf(SUB_PROCESS"Get AA ==> %02x %02x",ch,message_type);
					i=0;
					state=STATE_MESSAGE_TYPE;
				}
				break;
				case STATE_MESSAGE_TYPE:
				{
					message_type|=ch;
					state=STATE_MESSAGE_LEN;
				}
				break;
				case STATE_MESSAGE_LEN:
				{
					message_len=ch;
					state=STATE_MESSAGE;
					i=0;
				}
				break;
				case STATE_MESSAGE:
				{
					if(i!=message_len)
					{
						message[i++]=ch;
					}
					else
					{
						state=STATE_CRC;
						crc=ch<<8;
						rt_kprintf(SUB_PROCESS"crc1 %02x\n",ch);
					}	
				}
				break;
				case STATE_CRC:
				{
					crc|=ch;
					rt_kprintf(SUB_PROCESS"crc2 %02x\n",ch);
					rt_kprintf(SUB_PROCESS"GOT 0x6c 0xaa %04x %02x ",message_type,message_len);
					for(i=0;i<message_len;i++)
					{
						rt_kprintf("%02x ",message[i]);
						to_check[5+i]=message[i];
					}
					rt_kprintf("%04x \r\n",crc);
					to_check[0]=0x6c;to_check[1]=0xaa;to_check[2]=(message_type>>8)&0xff;to_check[3]=message_type&0xff;
					to_check[4]=message_len;to_check[5+message_len]=(crc>>8)&0xff;
					to_check[5+message_len+1]=crc&0xff;
					rt_kprintf(SUB_PROCESS"CRC Get %02x <> Count %02x\r\n",crc,CRC_check(to_check,message_len+5));
					if(crc==CRC_check(to_check,message_len+5))
					{
						if(post_message==NULL)
						{
							post_message=add_item(NULL,ID_DGRAM_TYPE,TYPE_DGRAM_DATA);
							post_message=add_item(post_message,ID_DEVICE_UID,"230FFEE9981283737D");
							post_message=add_item(post_message,ID_DEVICE_IP_ADDR,"192.168.1.2");
							post_message=add_item(post_message,ID_DEVICE_PORT,"9517");	
						}
						i=0;
						rt_memset(id,'\0',sizeof(id));
						rt_memset(data,'\0',sizeof(data));
						rt_memset(date,'\0',sizeof(date));
						rt_memset(error,'\0',sizeof(error));
						switch(message_type)
						{
							case TIME_BYTE:
								{
									rt_sprintf(date,"20%02d-%02d-%02d %02d:%02d",to_check[i+5],to_check[i+6],to_check[i+7],to_check[i+8],to_check[i+9],to_check[i+10]);
									rt_kprintf(SUB_PROCESS"date is %s\r\n",date);
									post_message=add_item(post_message,ID_DEVICE_CAP_TIME,date);
									can_send=1;
								}
								break;
							case ERROR_BYTE:
								{
									rt_sprintf(error,"%dth sensor possible error",to_check[i+2]);
									post_message=add_item(post_message,ID_ALERT_CAP_FAILED,error);
								}
								break;
							case RESEND_BYTE:
								{
									rt_device_write(dev_cap,0,server_time,13);
								}
								break;
							default:
								{
									/*get cap data*/
									if(to_check[i+5]==0x45 && to_check[i+6]==0x52 && to_check[i+7]==0x52 && to_check[i+8]==0x4f && to_check[i+9]==0x52)
									{
										rt_sprintf(error,"%dth sensor possible error",to_check[i+3]);
										post_message=add_item(post_message,ID_ALERT_CAP_FAILED,error);
									}
									else
									{
										rt_sprintf(id,"%d",message_type);
										rt_sprintf(data,"%d",to_check[i+5]<<8|to_check[i+6]);
										//rt_kprintf("pre data %s %d\r\n",data,rt_rt_strlen(data));
										
										if(to_check[i+7]>=rt_strlen(data))
										{									
											rt_sprintf(data,"0.%d%d",to_check[i+5],to_check[i+6]);
										}
										else if(to_check[i+7]==0)
										{
											if(to_check[i+5]!=0)
												rt_sprintf(data,"%d%d.0",to_check[i+5],to_check[i+6]);
											else
												rt_sprintf(data,"%d.0",to_check[i+6]);
										}
										else
										{
											for(j=rt_strlen(data);j>rt_strlen(data)-to_check[i+7]-1;j--)
											{
												data[j]=data[j-1];
												//rt_kprintf("j %d %c\r\n",j,data[j]);
											}	
											data[j]='.';
										}
										post_message=add_item(post_message,id,data);
										rt_kprintf(SUB_PROCESS"id %s data %s\r\n==>\n%s",id,data,post_message);
									}
								}
								break;
						}
					}
					else
					{
						rt_kprintf(SUB_PROCESS"CRC error \r\n");
						for(i=0;i<message_len+7;i++)
							rt_kprintf("0x%02x ",to_check[i]);
					}
					if(can_send)
					{
						can_send=0;
						#if 0
						j=0;
						for(i=0;i<rt_strlen(post_message);i++)
						{
							if(post_message[i]=='\n'||post_message[i]=='\r'||post_message[i]=='\t')
								j++;
						}
						rt_kprintf(SUB_PROCESS"send %d post_message %s",rt_strlen(post_message),post_message);
						char *out1=sram_malloc(rt_strlen(post_message)-j+1);
						rt_memset(out1,'\0',rt_strlen(post_message)-j+1);
						j=0;
						for(i=0;i<rt_strlen(post_message);i++)
						{
							if(post_message[i]!='\r'&&post_message[i]!='\n'&&post_message[i]!='\t')		
							{
								out1[j++]=post_message[i];
							}
						}
						#endif
						save_to_file(date,post_message);
						send_web_post(post_message,RT_WAITING_FOREVER);
						sram_free(post_message);
						//sram_free(post_message);
						post_message=NULL;
						if(rt_strlen(wifi_result)!=0 && rt_strncmp(wifi_result,"ok",rt_strlen("ok"))==0)
						{	
							int len=rt_strlen(wifi_result);
							rt_kprintf(SUB_PROCESS"<=== %s %d\n",wifi_result,len);
							rt_kprintf(SUB_PROCESS"send ok\n");
						}
						else
							rt_kprintf(SUB_PROCESS"send failed %s\r\n",wifi_result);
						rt_memset(wifi_result,0,512);
					}
					state=STATE_IDLE;
					i=0;
				}
				break;
				default:
				{
					i=0;
					state=STATE_IDLE;
				}
				break;
			}
		}		
	}	
}
/*dwin lcd process*/
static rt_err_t lcd_rx_ind(rt_device_t dev, rt_size_t size)
{
	rt_sem_release(&(lcd_rx_sem));    
	return RT_EOK;
}
void lcd_thread(void* parameter)
{	
	int len1=0,m=0;
	char *ptr=sram_malloc(32);			
	while(1)	
	{		
		if (rt_sem_take(&(lcd_rx_sem), RT_WAITING_FOREVER) != RT_EOK) continue;		
		int len=rt_device_read(dev_lcd, 0, ptr+m, 128);		
		if(len>0)	
		{	
			int i;		
			len1=len1+len;
			if(len1==9)
			{
				rt_kprintf("Get from Dwin Lcd:\n");
				for(i=0;i<len1;i++)		
				{		
					rt_kprintf("%x ",ptr[i]);
				}	
				data_ch2o=ptr[4]*256+ptr[5];
				rt_kprintf(" %d\n",data_ch2o);
				len1=0;
				m=0;
			}
			else
				m=m+len;
		}		
	}	
}

/*get data from server*/
static rt_err_t wifi_rx_ind(rt_device_t dev, rt_size_t size)
{
	rt_sem_release(&(wifi_rx_sem));    
	return RT_EOK;
}

void wifi_thread(void* parameter)
{	
	int len=0,i=0;
	char *ptr=(char *)sram_malloc(256);
	
	while(1)
	{		
		if (rt_sem_take(&(wifi_rx_sem), RT_WAITING_FOREVER) != RT_EOK) continue;	
		len=rt_device_read(dev_wifi, 0, ptr+i, 128);
		if(len>0)
		{
			int m;
			for(m=0;m<len;m++)
				rt_kprintf("%c",ptr[i+m]);
			rt_sem_release(&(server_sem));    
		}
		continue;
		if((len==1 && (ptr[0]=='+'||ptr[0]=='A'))||strstr(ptr,"+ERR")!=RT_NULL)
			continue;
		if(len>0)
		{
			i=i+len;
			if(rt_strstr(ptr,"ok")!=RT_NULL)
			{
				strcpy(wifi_result,"ok");
			}
			else if(/*strstr(ptr,"HTTP/1.1")!=RT_NULL && */strchr(ptr,'}')!=RT_NULL)	
			{	
				int j,m=0;	
				while(1)
				{
					if(ptr[m]=='H'&&ptr[m+1]=='T'&&ptr[m+2]=='T'&&ptr[m+3]=='P'&&ptr[m+4]=='/'&&ptr[m+5]=='1'&&ptr[m+6]=='.'&&ptr[m+7]=='1')
						break;
					m++;
				}
				if(m==i)
				{
					rt_memset(ptr,'\0',256);
					i=0;
					continue;
				}
				rt_kprintf("Get from Server:\n");
				for(j=m;j<i;j++)		
				{		
					rt_kprintf("%c",ptr[j]);
				}	
				rt_kprintf("\n");
				strcpy(wifi_result,ptr+m);
				#if 0
				char *result=http_parse_result(ptr+m);
				if(result!=RT_NULL)
				{
					char *id=doit_data(result,"30");
					char *start=doit_data(result,"101");
					char *stop=doit_data(result,"102");
					//rt_kprintf("result is %s\n",result);
					if(id!=RT_NULL)
					{
						rt_kprintf("ID %s\n",id);
						sram_free(id);
					}
					if(start!=RT_NULL)
					{
						rt_kprintf("start time %s\n",start);
						sram_free(start);
					}
					if(stop!=RT_NULL)
					{
						rt_kprintf("stop time %s\n",stop);
						sram_free(stop);
					}
					sram_free(result);
				}
				#endif
			}		
			rt_memset(ptr,'\0',256);
			i=0;		
		}
	}	
}

//uart1 for debug
//uart2 for wifi
//uart3 for cap
//uart4 for lcd
int init_cap()
{
	sram_init();
	dev_cap=rt_device_find("uart3");
	if (rt_device_open(dev_cap, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) == RT_EOK)			
	{
		struct serial_configure config;			
		config.baud_rate=9600;
		config.bit_order = BIT_ORDER_LSB;			
		config.data_bits = DATA_BITS_8;			
		config.parity	 = PARITY_NONE;			
		config.stop_bits = STOP_BITS_1;				
		config.invert	 = NRZ_NORMAL;				
		config.bufsz	 = RT_SERIAL_RB_BUFSZ;			
		rt_device_control(dev_cap,RT_DEVICE_CTRL_CONFIG,&config);	
		rt_sem_init(&(cap_rx_sem), "cap_rx", 0, 0);
		rt_device_set_rx_indicate(dev_cap, cap_rx_ind);
		rt_thread_startup(rt_thread_create("thread_cap",cap_thread, 0,2048, 20, 10));
		//rt_device_write(dev_cap, 0, (void *)read_co2, 9);
		#if 0
		post_message=add_item(RT_NULL,ID_DGRAM_TYPE,TYPE_DGRAM_DATA);
		post_message=add_item(post_message,ID_DEVICE_UID,"230FFEE9981283737D");
		post_message=add_item(post_message,ID_DEVICE_IP_ADDR,"192.168.1.2");
		post_message=add_item(post_message,ID_DEVICE_PORT,"9517");
		post_message=add_item(post_message,"ff","9513.0");
		post_message=add_item(post_message,"12","9512.00");
		post_message=add_item(post_message,"32","9511.df");
		rt_kprintf("==>\n%s",post_message);
		sram_free(post_message);
		#endif
	}
	else
	{
		rt_kprintf("open cap board uart3 failed\r\n");
		return -1;
	}
	dev_lcd=rt_device_find("uart4");
	if (rt_device_open(dev_lcd, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) == RT_EOK)			
	{
		struct serial_configure config;			
		config.baud_rate=115200;
		config.bit_order = BIT_ORDER_LSB;			
		config.data_bits = DATA_BITS_8;			
		config.parity	 = PARITY_NONE;			
		config.stop_bits = STOP_BITS_1;				
		config.invert	 = NRZ_NORMAL;				
		config.bufsz	 = RT_SERIAL_RB_BUFSZ;			
		rt_device_control(dev_lcd,RT_DEVICE_CTRL_CONFIG,&config);	
		rt_sem_init(&(lcd_rx_sem), "lcd_rx", 0, 0);
		rt_device_set_rx_indicate(dev_lcd, lcd_rx_ind);
		rt_thread_startup(rt_thread_create("thread_lcd",lcd_thread, 0,512, 20, 10));
	}
	else
	{
		rt_kprintf("open dwin lcd uart4 failed\r\n");
		return -1;
	}
	dev_wifi=rt_device_find("uart2");
	if (rt_device_open(dev_wifi, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) == RT_EOK)			
	{			
		unsigned char switch_at='+';
		unsigned char done='a';
		char *cmd;
		rt_sem_init(&(server_sem), "server_rx", 0, 0);
		rt_sem_init(&(wifi_rx_sem), "wifi_rx", 0, 0);
		rt_device_set_rx_indicate(dev_wifi, wifi_rx_ind);
		rt_thread_startup(rt_thread_create("thread_wifi",wifi_thread, 0,512, 20, 10));

		rt_thread_delay(200);	
		rt_device_write(dev_wifi, 0, (void *)&switch_at, 1);
		rt_thread_delay(3);
		rt_device_write(dev_wifi, 0, (void *)&switch_at, 1);
		rt_thread_delay(3);
		rt_device_write(dev_wifi, 0, (void *)&switch_at, 1);
		rt_thread_delay(3);
		rt_device_write(dev_wifi, 0, (void *)&done, 1);
		rt_thread_delay(10);
		cmd=(char *)sram_malloc(512);
		rt_memset(cmd,0,512);
		strcpy(cmd,"AT+HTPFT=OFF\n");
		rt_device_write(dev_wifi, 0, (void *)cmd, rt_strlen(cmd));
		rt_thread_delay(10);
		rt_memset(cmd,0,512);
		strcpy(cmd,"AT+HTPTP=POST\n");
		rt_device_write(dev_wifi, 0, (void *)cmd, rt_strlen(cmd));
		rt_thread_delay(10);
		rt_memset(cmd,0,512);
		strcpy(cmd,"AT+HTPSV=101.200.182.92,8080\n");
		//strcpy(cmd,"AT+HTPSV=16.168.0.3,8080\n");
		rt_device_write(dev_wifi, 0, (void *)cmd, rt_strlen(cmd));
		rt_thread_delay(10);
		rt_memset(cmd,0,512);
		strcpy(cmd,"AT+HTPHD=Connection: Keep-Alive[0D][0A]Content-Type:application/x-www-form-urlencoded[0D][0A]\n");
		rt_device_write(dev_wifi, 0, (void *)cmd, rt_strlen(cmd));
		rt_thread_delay(10);
		rt_memset(cmd,0,512);
		strcpy(cmd,"AT+HTPURL=/saveData/airmessage/messMgr.do[3F]\n");
		rt_device_write(dev_wifi, 0, (void *)cmd, rt_strlen(cmd));
		rt_thread_delay(10);
		rt_memset(cmd,0,512);
		strcpy(cmd,"AT+Z\n");
		rt_device_write(dev_wifi, 0, (void *)cmd, rt_strlen(cmd));
		rt_thread_delay(300);
		//sram_free(cmd);
		//cmd=(char *)sram_malloc(512);
		rt_memset(cmd,0,512);
		//strcpy(cmd,"JSONStr={\"0\":\"2\",\"30\":\"1234abcd\",\"35\":\"192.1.3.21\",\"36\":\"8769\",\"60\":\"34\",\"61\":\"54\",\"62\":\"14\",\"63\":\"54\",\"64\":\"120\",\"65\":\"121\",\"90\":\"92\",\"103\":\"2015-10-20 12:16\"}");
		strcpy(cmd,"JSONStr={\"0\":\"5\",\"30\":\"230FFEE9981283737D\",\"35\":\"192.1.3.55\",\"36\":\"8769\"}");
		rt_device_write(dev_wifi, 0, (void *)cmd, rt_strlen(cmd));
		rt_kprintf("send %s\n",cmd);
		//rt_thread_delay(200);
		//rt_device_write(dev_wifi, 0, (void *)cmd, rt_strlen(cmd));
		//rt_thread_delay(200);
		//rt_device_write(dev_wifi, 0, (void *)cmd, rt_strlen(cmd));
		sram_free(cmd);
	}
	else
	{
		rt_kprintf("open youren wifi uart2 failed\r\n");
		return -1;
	}
	return 0;
#if 0
	while(1){
		//rt_kprintf("cur str is %s\n",str);
		rt_device_write(dev_cap, 0, (void *)read_co2, 9);
		rt_sprintf(str,"%03d",data_co2);
		rt_sprintf(str1,"%03d",data_ch2o);
		clear();
		draw(str,str1);
		display();		
		post_message=RT_NULL;
		//get device uid,ip,port,cap data,cap time send to server
		//post_message=add_item(NULL,ID_DGRAM_TYPE,TYPE_DGRAM_DATA);
		//post_message=add_item(post_message,ID_DEVICE_UID,"230FFEE9981283737D");
		//post_message=add_item(post_message,ID_DEVICE_IP_ADDR,"192.168.1.63");
		//post_message=add_item(post_message,ID_DEVICE_PORT,"6547");
		if(flag)
			post_message=add_item(post_message,ID_CAP_CO2,str);
		else
		post_message=add_item(post_message,ID_CAP_HCHO,str1);
		flag=!flag;
		//post_message=add_item(post_message,id1,data1);
		//post_message=add_item(post_message,ID_DEVICE_CAP_TIME,"2015-08-06 00:00");
		int i,j=0;
		for(i=0;i<rt_strlen(post_message);i++)
		{
			if(post_message[i]==',')
				j++;
		}
		char *out1=sram_malloc(rt_strlen(post_message)+3*j+1);
		rt_memset(out1,'\0',rt_strlen(post_message)+3*j+1);
		//out1[0]='"';
		j=0;
		for(i=0;i<rt_strlen(post_message);i++)
		{
			if(post_message[i]!='\r'&&post_message[i]!='\n'&&post_message[i]!=9)
			//{
			//	out1[j++]='[';out1[j++]='0';out1[j++]='D';out1[j++]=']';
			//}
			//else if(post_message[i]=='\n')
			//{
			//	out1[j++]='[';out1[j++]='0';out1[j++]='A';out1[j++]=']';
			//} 
			//else
			{
				if(post_message[i]==',')
				{
					out1[j++]='[';out1[j++]='2';out1[j++]='C';out1[j++]=']';
				}
				else
				out1[j++]=post_message[i];
			}
		}
		//out1[j]='"';
		sram_free(post_message);
		char *send=(char *)sram_malloc(rt_strlen(out1)+rt_strlen(httpd_local)+1+1);
		rt_memset(send,'\0',rt_strlen(out1)+rt_strlen(httpd_local)+1+1);
		strcpy(send,httpd_local);
		strcat(send,out1);
		strcat(send,"\n");
		rt_kprintf("send %s",send);
		rt_device_write(dev_wifi, 0, (void *)send, rt_strlen(send));
		rt_thread_delay(10);
		rt_device_write(dev_wifi, 0, (void *)httpd_send, rt_strlen(httpd_send));
		sram_free(send);
		sram_free(out1);
		//rt_sem_take(&(server_sem), RT_WAITING_FOREVER);
		rt_thread_delay(600);
		}
#endif
}
#ifdef RT_USING_FINSH
#include <finsh.h>

void wifi(char *arg)
{
	char *cmd=(char *)sram_malloc(strlen(arg)+3);
	rt_memset(cmd,0,strlen(arg)+2);
	strcpy(cmd,arg);
	strcat(cmd,"\n");
	rt_kprintf("%s",cmd);
	rt_device_write(dev_wifi, 0, (void *)arg, rt_strlen(arg));
	sram_free(cmd);
}
void rst()
{
	NVIC_SystemReset();
}
void cat1(char *file)
{
	int fp;
	char *data;
	data=(char *)sram_malloc(256);
	rt_memset(data,0,256);
	fp = open(file, O_RDONLY,0);
	if (fp < 0)
	{
			rt_kprintf("can not create %s\r\n",file);
			sram_free(data);
			return;
	}
	while(1)
	{		
		if(read(fp,data,256)>0)
		{
			rt_kprintf("%s",data);
			rt_memset(data,0,256);
		}
		else
			break;
	}
	close(fp);
	sram_free(data);
}

FINSH_FUNCTION_EXPORT(cat1, cat file)
FINSH_FUNCTION_EXPORT(wifi, wifi cmd)
FINSH_FUNCTION_EXPORT(rst, system reset)
#endif

