#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <dfs_posix.h>
#include "rtc.h"
#ifdef  RT_USING_COMPONENTS_INIT
#include <components.h>
#endif  /* RT_USING_COMPONENTS_INIT */
#include "cJSON.h"
#include "cap.h"
//#define SUB_PROCESS "[CapProcess]"
struct rt_semaphore cap_rx_sem,wifi_rx_sem,server_sem,lcd_rx_sem,alarm_sem;;
rt_device_t dev_cap,dev_wifi,dev_lcd,dev_gprs;
int data_co2=0,data_ch2o;
char *post_message=NULL,can_send=0;
char *server_time=RT_NULL;
char *wifi_result=RT_NULL;
//struct rt_mutex     lock;
rt_bool_t sync=RT_FALSE;
unsigned short co,co2,hcho,temp,shidu,pm25;
char *http_parse_result(const char*lpbuf);
#define SRAM_MAPPING_ADDRESS 0x10000000
struct rt_memheap system_heap;
int g_index=0;
rt_bool_t server_time_got=RT_FALSE,send_by_wifi=RT_FALSE;
void write_data(unsigned int Index,int data);
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
#if 0
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
#endif
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
#if 0
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
#endif
void send_web_post(rt_bool_t wifi,char *log,char *buf,int timeout)
{
	int i,j,ltimeout=0;
	char ch;
	rt_device_t dev;
	rt_err_t rt;
	if(wifi)
	{	
		char *httpd_send=(char *)sram_malloc(strlen(buf)+strlen("JSONStr=")+1);
		strcpy(httpd_send,"JSONStr=");
		strcat(httpd_send,buf);
		rt_device_write(dev_wifi, 0, (void *)httpd_send, rt_strlen(httpd_send));	
		dev=dev_wifi;
		sram_free(httpd_send);		
		rt=rt_sem_take(&(wifi_rx_sem), 700);			
		i=1;
		j=0;
		if(rt!=RT_EOK)
		{
			rt_kprintf("no rcv ,timeout\n");
			return;
		}
		if(timeout==-1)
		{
			while(rt_device_read(dev, 0, &ch, 1)==0);
		}
		else
		{
			while(rt_device_read(dev, 0, &ch, 1)==0)
			{
				ltimeout++;
				rt_thread_delay(1);
				if(ltimeout>=timeout)
				{
					rt_kprintf("first timeout\n");
					return;
				}
			}
		}
		if(ch=='{')
		{
			wifi_result[0]='{';
			while(1)
			{
				if(timeout==-1)
				{
					while(rt_device_read(dev, 0, &ch, 1)==0);
				}
				else
				{
					ltimeout=0;
					while(rt_device_read(dev, 0, &ch, 1)==0)
					{
						ltimeout++;
						rt_thread_delay(1);
						if(ltimeout>=timeout)
						{
							rt_kprintf("second timeout\n");
							return;
						}
					}
				}
				if(ch=='}')
				{
					wifi_result[i++]='}';
					break;
				}
				else
					wifi_result[i++]=ch;
			}
		}
		else if(ch=='o')
		{
			strcpy(wifi_result,"ok");
			i=3;
		}
		else 
		{
			strcpy(wifi_result,"failed");
			rt_kprintf("\n<%c>\n",ch);
			i=7;
		}
	}
	else
	{
		char *gprs_string=(char *)sram_malloc(strlen(buf)+strlen("/saveData/airmessage/messMgr.do?JSONStr=")+rt_strlen("\r\nHTTP/ 1.1\r\nhost:101.200.182.92")+1);
		strcpy(gprs_string,"/saveData/airmessage/messMgr.do?JSONStr=");
		strcat(gprs_string,buf);
		strcat(gprs_string,"\r\nHTTP/ 1.1\r\nhost:101.200.182.92");
		rt_memset(wifi_result,'v',512);
		rt_device_write(dev_gprs, 0, (void *)gprs_string, rt_strlen(gprs_string));	
		dev=dev_gprs;
		rt=rt_sem_take(&(server_sem), 700);
		if(rt==RT_EOK)
		{
			while(rt_device_read(dev, 0, &ch, 1)==1)
				rt_kprintf("%c",ch);			
		}
		else
			rt_kprintf("GPRS rcv timeout\n");
		sram_free(gprs_string);
	}
	rt_kprintf("\n=====================>\n");
	//for(j=0;j<rt_strlen(httpd_send);j++)
	//	rt_kprintf("%c",httpd_send[j]);
	//rt_kprintf("\nsend done.\n");
	for(j=0;j<i;j++)
		rt_kprintf("%c",wifi_result[j]);
	while(rt_device_read(dev_wifi, 0, &ch, 1)==1);
	rt_kprintf("\n=====================>\n");
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
	int fp;
	char *file_path=RT_NULL;
	char *data=RT_NULL;
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
			rt_kprintf(RESEND_PROCESS"can not create %s\r\n",file_path);
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
	FILE *fp=RT_NULL;
	int j;
	int month_b,year_b,day_b,month_e,year_e,day_e,hour_e,minute_e,max_day;	
	char * line = NULL;
	char *year_begin=RT_NULL;
	char *year_end=RT_NULL;
	char *month_begin=RT_NULL;
	char *month_end=RT_NULL;
	char *day_begin=RT_NULL;
	char *day_end=RT_NULL;
	char *hour_end=RT_NULL;
	char *minute_end=RT_NULL;
	char *file_path=RT_NULL;
	char *date=RT_NULL;
	file_path=(char *)sram_malloc(256);
	date=(char *)sram_malloc(32);
	minute_end=(char *)sram_malloc(3);
	hour_end=(char *)sram_malloc(3);
	day_end=(char *)sram_malloc(3);
	day_begin=(char *)sram_malloc(3);
	month_end=(char *)sram_malloc(3);
	month_begin=(char *)sram_malloc(3);
	year_end=(char *)sram_malloc(5);
	year_begin=(char *)sram_malloc(5);
	rt_memset(file_path,0,256);
	rt_memset(date,0,32);
	rt_memset(minute_end,0,3);
	rt_memset(hour_end,0,3);
	rt_memset(day_end,0,3);
	rt_memset(day_begin,0,3);
	rt_memset(month_end,0,3);
	rt_memset(month_begin,0,3);
	rt_memset(year_end,0,5);
	rt_memset(year_begin,0,5);
	
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
	line=(char *)sram_malloc(512);
	rt_kprintf(RESEND_PROCESS"year_b %04d,month_b %02d,day_b %02d,year_e %04d,month_e %02d,day_e %02d\r\n",year_b,month_b,day_b,year_e,month_e,day_e);
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
			rt_kprintf(RESEND_PROCESS"to open %s\r\n",file_path);
			fp = fopen(file_path, "r");
			if (fp != RT_NULL)
			{
				int tmp_i=0;
				//size_t len = 0;
				rt_kprintf(RESEND_PROCESS"open file %s ok\r\n",file_path);
				while (fgets(line,512,fp)) 
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
								rt_kprintf(RESEND_PROCESS"file_time %02d:%02d,end time %02d:%02d",atoi(local_hour),atoi(local_minute),hour_e,minute_e);
								sram_free(line);
								fclose(fp);
								sram_free(file_path);
								sram_free(date);
								sram_free(minute_end);
								sram_free(hour_end);
								sram_free(day_end);
								sram_free(month_end);
								sram_free(month_begin);
								sram_free(year_end);
								sram_free(year_begin);
								sram_free(line);
								return;
							}
						}
						else
						{
							line[rt_strlen(line)-1]='\0';				
							for(j=0;j<rt_strlen(line);j++)
								rt_kprintf("%c",line[j]);
							rt_kprintf("\nsend done.\n");
							//rt_mutex_take(&lock, RT_WAITING_FOREVER);
							rt_kprintf(RESEND_PROCESS"rsend web:\n");
							send_web_post(send_by_wifi,RESEND_PROCESS,line,1000);
							//rt_mutex_release(&lock);
						}
					}
					else
					{
						if((tmp_i%2)!=0)
						{						
							line[rt_strlen(line)-1]='\0';
							//rt_mutex_take(&lock, RT_WAITING_FOREVER);
							rt_kprintf(RESEND_PROCESS"rsend web:\n");							
							for(j=0;j<rt_strlen(line);j++)
								rt_kprintf("%c",line[j]);
							rt_kprintf("\nsend done.\n");
							send_web_post(send_by_wifi,RESEND_PROCESS,line,1000);
							//rt_mutex_release(&lock);							
						}
					}
					tmp_i++;
				}
				sram_free(line);
					
			}
			else
			{
				rt_kprintf(RESEND_PROCESS"can not open %s\r\n",file_path);
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
			rt_kprintf(RESEND_PROCESS"end year_b %04d,month_b %02d,day_b %02d,year_e %04d,month_e %02d,day_e %02d\r\n",year_b,month_b,day_b,year_e,month_e,day_e);
			break;
		}
	}
	if(fp!=RT_NULL)
	fclose(fp);
	sram_free(file_path);
	sram_free(date);
	sram_free(minute_end);
	sram_free(hour_end);
	sram_free(day_end);
	sram_free(month_end);
	sram_free(month_begin);
	sram_free(year_end);
	sram_free(year_begin);
	//sram_free(line);
}

void sync_server(rt_device_t fd,int resend)
{
	int i,j;
	//char text_out[512]={0};
	char *sync_message=NULL;
	if(resend)
		sync_message=add_item(NULL,ID_DGRAM_TYPE,TYPE_DGRAM_ASK_RE_DATA);
	else
		sync_message=add_item(NULL,ID_DGRAM_TYPE,TYPE_DGRAM_SYNC);
	sync_message=add_item(sync_message,ID_DEVICE_UID,"230FFEE9981283737D");
	sync_message=add_item(sync_message,ID_DEVICE_IP_ADDR,"16.168.1.23");
	sync_message=add_item(sync_message,ID_DEVICE_PORT,"9517");
	rt_kprintf(ALARM_PROCESS"<sync GET>:\n");
	//rt_mutex_take(&lock, RT_WAITING_FOREVER);	
	for(j=0;j<rt_strlen(sync_message);j++)
		rt_kprintf("%c",sync_message[j]);
	rt_kprintf("\nsend done.\n");
	send_web_post(send_by_wifi,ALARM_PROCESS,sync_message,-1);
	sram_free(sync_message);
	if(rt_strlen(wifi_result)!=0)
	{	
		//int len=rt_strlen(wifi_result);
		//rt_kprintf(ALARM_PROCESS"<=== %s %d\n",wifi_result,len);
		//rt_kprintf(ALARM_PROCESS"send ok\n");
		char *starttime=NULL;
		char *tmp=NULL;		
		if(resend)
		{
			starttime=doit_data(wifi_result,(char *)"101");
			tmp=doit_data(wifi_result,(char *)"102");			
			//rt_mutex_release(&lock);
			if(starttime!=NULL && tmp!=NULL)
			{
				rt_kprintf(ALARM_PROCESS"%s\r\n",tmp);
				rt_kprintf(ALARM_PROCESS"%s\r\n",starttime);
				//resend_history(starttime,tmp);
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
				starttime=doit_data(wifi_result,(char *)"104");
				if(starttime!=RT_NULL)
				{
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
					for(i=0;i<13;i++)
					{
						rt_kprintf("%02x\n",server_time[i]);
					}
					rt_device_write(fd, 0, (void *)server_time, 13);
					rt_kprintf(ALARM_PROCESS"SERVER TIME %s\r\n",starttime);
					//rt_kprintf(ALARM_PROCESS"==>%d:%d:%d %d_%d_%d\n",server_time[5],server_time[6],server_time[7],server_time[8],server_time[9],server_time[10]);
					//tmp=doit_data(rcv+4,(char *)"211");
					rt_kprintf(ALARM_PROCESS"211 %s\r\n",doit_data(wifi_result,"211"));
					rt_kprintf(ALARM_PROCESS"212 %s\r\n",doit_data(wifi_result,"212"));
					rt_kprintf("to settime %d.%d.%d-%d:%d:%d\n",atoi(year)+2000,atoi(month),atoi(day),atoi(hour),atoi(minute),atoi(second));
					set_date(atoi(year)+2000,atoi(month),atoi(day));
					set_time(atoi(hour),atoi(minute),atoi(second));	
					server_time_got=RT_TRUE;
				}
				//rt_mutex_release(&lock);
			//}
			//else if(atoi(type)==6)
			//{
			//}
		}
		//sram_free(rcv);
	}
	else
	{
		rt_kprintf("==>%s\n",wifi_result);
		//rt_mutex_release(&lock);
	}
	rt_memset(wifi_result,0,512);
	g_index=0;
	
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
					//rt_kprintf(SUB_PROCESS"Get AA ==> %02x %02x",ch,message_type);
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
						//rt_kprintf(SUB_PROCESS"crc1 %02x\n",ch);
					}	
				}
				break;
				case STATE_CRC:
				{
					crc|=ch;
					//rt_kprintf(SUB_PROCESS"crc2 %02x\n",ch);
					//rt_kprintf(SUB_PROCESS"GOT 0x6c 0xaa %04x %02x ",message_type,message_len);
					for(i=0;i<message_len;i++)
					{
						//rt_kprintf("%02x ",message[i]);
						to_check[5+i]=message[i];
					}
					//rt_kprintf("%04x \r\n",crc);
					to_check[0]=0x6c;to_check[1]=0xaa;to_check[2]=(message_type>>8)&0xff;to_check[3]=message_type&0xff;
					to_check[4]=message_len;to_check[5+message_len]=(crc>>8)&0xff;
					to_check[5+message_len+1]=crc&0xff;
					//rt_kprintf(SUB_PROCESS"CRC Get %02x <> Count %02x\r\n",crc,CRC_check(to_check,message_len+5));
					if(crc==CRC_check(to_check,message_len+5))
					{						
						i=0;
						rt_memset(id,'\0',sizeof(id));
						rt_memset(data,'\0',sizeof(data));
						rt_memset(date,'\0',sizeof(date));
						rt_memset(error,'\0',sizeof(error));
						switch(message_type)
						{
							case TIME_BYTE:
								{
									#if 0
									time_t now;
									struct tm* rtc_tm;
									time(&now);
									rtc_tm = localtime(&now);
									rt_sprintf(date,"%02d-%02d-%02d %02d:%02d",rtc_tm->tm_year+1900,rtc_tm->tm_mon+1,rtc_tm->tm_mday,rtc_tm->tm_hour,rtc_tm->tm_min);
									#else
									if(send_by_wifi)
									rt_sprintf(date,"20%02d-%02d-%02d %02d:%02d",to_check[i+5],to_check[i+6],to_check[i+7],to_check[i+8],to_check[i+9],to_check[i+10]);
									else
									rt_sprintf(date,"20%02d-%02d-%02d%%20%02d:%02d",to_check[i+5],to_check[i+6],to_check[i+7],to_check[i+8],to_check[i+9],to_check[i+10]);		
									#endif
									rt_kprintf(SUB_PROCESS"date is %s\r\n",date);
									list_date();
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
										if(post_message==NULL)
										{
											post_message=add_item(NULL,ID_DGRAM_TYPE,TYPE_DGRAM_WARNING);
											post_message=add_item(post_message,ID_DEVICE_UID,"230FFEE9981283737D");
											post_message=add_item(post_message,ID_DEVICE_IP_ADDR,"192.168.1.2");
											post_message=add_item(post_message,ID_DEVICE_PORT,"9517");
											can_send=1;
										}
									}
									else
									{
										//int high,low=0;
										if(post_message==NULL)
										{
											post_message=add_item(NULL,ID_DGRAM_TYPE,TYPE_DGRAM_DATA);
											post_message=add_item(post_message,ID_DEVICE_UID,"230FFEE9981283737D");
											post_message=add_item(post_message,ID_DEVICE_IP_ADDR,"192.168.1.2");
											post_message=add_item(post_message,ID_DEVICE_PORT,"9517");
										}
										#if 0
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
										#else										
										rt_sprintf(id,"%d",message_type);
										rt_sprintf(data,"%d",to_check[i+5]<<8|to_check[i+6]);
										//high=to_check[i+5]<<8|to_check[i+6];										
										if(to_check[i+7]!=0)
										{//have .
											int m;
											if(to_check[i+7]>rt_strlen(data))
											{
												char tmp_buf[10]={0};
												int dist=to_check[i+7]-rt_strlen(data);
												strcpy(tmp_buf,"0.");
												for(m=0;m<dist;m++)
													strcat(tmp_buf,"0");
												strcat(tmp_buf,data);
												strcpy(data,tmp_buf);
												//high=0;
												//low=to_check[i+5]<<8|to_check[i+6];
											}
											else
											{
												int left,right,number,n=1;
												number=(to_check[i+5]<<8)|to_check[i+6];
												for(m=0;m<to_check[i+7];m++)
													n=n*10;
												right=number%n;
												left=number/n;
												rt_sprintf(data,"%d.%d",left,right);
												//high=left;low=right;												
											}
										}
										#endif
										if(strncmp(id,ID_CAP_CO,strlen(ID_CAP_CO))==0)
										{
											//co=atoi(data);
											write_data(VAR_DATE_TIME_1,to_check[i+5]<<8|to_check[i+6]);
											//write_data(VAR_ALARM_TYPE_3,999);
											//write_data(VAR_ALARM_TYPE_4,999);			
										}
										else if(strncmp(id,ID_CAP_CO2,strlen(ID_CAP_CO2))==0)
										{
											//co2=atoi(data);											
											write_data(VAR_DATE_TIME_2,to_check[i+5]<<8|to_check[i+6]);
										}
										else if(strncmp(id,ID_CAP_HCHO,strlen(ID_CAP_HCHO))==0)
										{
											//hcho=atoi(data);
											write_data(VAR_DATE_TIME_3,to_check[i+5]<<8|to_check[i+6]);
										}
										else if(strncmp(id,ID_CAP_TEMPERATURE,strlen(ID_CAP_TEMPERATURE))==0)
										{
											//temp=atoi(data);
											write_data(VAR_DATE_TIME_4,to_check[i+5]<<8|to_check[i+6]);
										}
										else if(strncmp(id,ID_CAP_SHI_DU,strlen(ID_CAP_SHI_DU))==0)
										{
											//shidu=atoi(data);
											write_data(VAR_ALARM_TYPE_1,to_check[i+5]<<8|to_check[i+6]);
										}
										else if(strncmp(id,ID_CAP_PM_25,strlen(ID_CAP_PM_25))==0)
										{
											//pm25=atoi(data);
											write_data(VAR_ALARM_TYPE_2,to_check[i+5]<<8|to_check[i+6]);
										}
										post_message=add_item(post_message,id,data);
										//rt_kprintf(SUB_PROCESS"id %s data %s\r\n==>\n%s\n",id,data,post_message);
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
						save_to_file(date,post_message);						
						for(j=0;j<rt_strlen(post_message);j++)
							rt_kprintf("%c",post_message[j]);
						rt_kprintf("\nsend done.\n");
						//rt_mutex_take(&lock, RT_WAITING_FOREVER);
						if(server_time_got)
						send_web_post(send_by_wifi,SUB_PROCESS,post_message,1000);
						sram_free(post_message);
						post_message=NULL;
						//rt_mutex_release(&lock);
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


unsigned short input_handle(char *input)
{
	int addr=0,data=0;
	rt_kprintf("got press\r\n");
	input[0]=2;
	addr=input[1]<<8|input[2];
	data=input[4]<<8|input[5];
	switch(addr)
	{
		
		case TOUCH_DETAIL_CO:
			if((TOUCH_DETAIL_CO-0x100)==data)
				return STATE_MAIN;
			/*
		case START_PRESS:
			if(START_DATA==data)
				return STATE_START;
		case S1_PRESS:
			if(S1_DATA==data)
				return STATE_MAIN_S1;
		case S2_PRESS:
		if(S2_DATA==data)
			return STATE_MAIN_S2;
		case S3_PRESS:
		if(S3_DATA==data)
			return STATE_MAIN_S3;
		case S4_PRESS:
		if(S4_DATA==data)
			return STATE_MAIN_S4;
		case S5_PRESS:
		if(S5_DATA==data)
			return STATE_MAIN_S5;
		case S6_PRESS:
		if(S6_DATA==data)
			return STATE_MAIN_S6;
		case S7_PRESS:
		if(S7_DATA==data)
			return STATE_MAIN_S7;
		case S8_PRESS:
		if(S8_DATA==data)
			return STATE_MAIN_S8;
		default:
			return STATE_MAIN;*/
		
	}
	return STATE_MAIN;
}

void switch_pic(unsigned char Index)	//??????????
{
	char cmd[]={0x5a,0xa5,0x03,0x80,0x04,0x00};
	cmd[5]=Index;
	rt_device_write(dev_lcd,0,cmd,6);
}
void write_data(unsigned int Index,int data)	//??????????
{
	char cmd[]={0x5a,0xa5,0x05,0x82,0x00,0x00,0x00,0x00};
	cmd[4]=(Index&0xff00)>>8;cmd[5]=Index&0x00ff;
	cmd[6]=(data&0xff00)>>8;cmd[7]=data&0x00ff;
	rt_device_write(dev_lcd,0,cmd,8);
}
void lcd_ctl(int state)
{
#if 0
	switch(state)
	{
		case STATE_ORIGIN:
		{
			rt_kprintf("STATE_ORIGIN state \n");
			switch_pic(0);
			break;
		}
		case STATE_MAIN:
		{
			rt_kprintf("STATE_MAIN\n");				
			break;
		}			
		case STATE_ALARM_INFO:
		{
			rt_kprintf("STATE_ALARM_INFO\n");		
			break;
		}		
		case STATE_START:
		{
			rt_kprintf("STATE_START \n");	
			break;
		}	
		case STATE_MAIN_S1:
		case STATE_MAIN_S2:
		case STATE_MAIN_S3:
		case STATE_MAIN_S4:
		case STATE_MAIN_S5:
		case STATE_MAIN_S6:
		case STATE_MAIN_S7:
		case STATE_MAIN_S8:
		{
			//write_data(VAR_CHANNEL_SET,state-4);
		}
		default:					
		{
			rt_kprintf("UNKNOWN  state , got STATE_ORIGIN\r\n");
			break;
		}
	}
#endif
}
void lcd_thread(void* parameter)
{	
	//int len1=0,m=0;
	char ch;
	int i=1;
	int get=0;
	char *ptr=sram_malloc(32);	
	switch_pic(0);
	while(1)	
	{		
		if (rt_sem_take(&(lcd_rx_sem), RT_WAITING_FOREVER) != RT_EOK) continue;		
		#if 0
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
		#else
		while (rt_device_read(dev_lcd, 0, &ch, 1) == 1)
		{
			//rt_kprintf("<=%x \r\n",ch);
			switch(get)
			{
				case 0:
					if(ch==0x5a)
					{
						//rt_kprintf("0x5a get ,get =1\r\n");
						get=1;
					}
					break;
				case 1:
					if(ch==0xa5)
					{
						//rt_kprintf("0xa5 get ,get =2\r\n");
						get=2;
						
						}
					break;
				case 2:
					if(ch==0x06)
					{
						//rt_kprintf("0x06 get,get =3\r\n");
						get=3;
						break;
					}
				case 3:
					if(ch==0x83)
					{
						//rt_kprintf("0x83 get,get =4\r\n");
						get=4;
						i=1;
						break;
					}
				case 4:
					{
						//rt_kprintf("%02x get ,get =5\r\n",ch);
						ptr[i++]=ch;
						if(i==6)
						{
							get=0;
							ptr[0]=0x01;
							rt_kprintf("get %x %x %x %x %x %x\r\n",ptr[0],ptr[1],ptr[2],ptr[3],ptr[4],ptr[5]);
							lcd_ctl(input_handle(ptr));
							
						}
					}
					break;			
				default:
					rt_kprintf("unknown state\r\n");
					break;						
			}			
		}
		#endif
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
//	int i=1,j;
	char ch;
	while(1)
	{		
		if (rt_sem_take(&(server_sem), RT_WAITING_FOREVER) != RT_EOK) continue;
		while(rt_device_read(dev_gprs, 0, &ch, 1)==1)
			rt_kprintf("%c",ch);
		#if 0
		if(ch=='{')
		{
			wifi_result[0]='{';
			while(1)
			{
				while(rt_device_read(dev_wifi, 0, &ch, 1)==0);
				
				if(ch=='}')
				{
					wifi_result[i++]='}';
					break;
				}
				else
					wifi_result[i++]=ch;
			}
		}
		else if(ch=='o')
		{
			strcpy(wifi_result,"ok");
			i=3;
		}
		else 
		{
			strcpy(wifi_result,"failed");
			i=7;
		}
		rt_kprintf("\n=====================>\n");
		for(j=0;j<i;j++)
			rt_kprintf("%c",wifi_result[j]);
		rt_kprintf("\n=====================>\n");
		#endif
	}	
}
void set_next_alarm(int step)
{
	time_t now;
	struct tm* rtc_tm;

	time(&now);
	rtc_tm = localtime(&now);
	rtc_tm->tm_sec += step;
	if (rtc_tm->tm_sec >= 60) {
	rtc_tm->tm_sec %= 60;
	rtc_tm->tm_min++;
	}
	if (rtc_tm->tm_min == 60) {
	rtc_tm->tm_min = 0;
	rtc_tm->tm_hour++;
	}
	if (rtc_tm->tm_hour == 24)
	rtc_tm->tm_hour = 0;

	rtc_tm->tm_min +=0;
	if (rtc_tm->tm_min == 60) {
	rtc_tm->tm_min = 0;
	rtc_tm->tm_hour++;
	}
	if (rtc_tm->tm_hour == 24)
	rtc_tm->tm_hour = 0;

	rtc_tm->tm_hour +=0;
	if (rtc_tm->tm_hour == 24)
	rtc_tm->tm_hour = 0;
	rt_kprintf(ALARM_PROCESS"tm_hour %02d,tm_min %02d,tm_sec %02d\r\n",rtc_tm->tm_hour,rtc_tm->tm_min,rtc_tm->tm_sec);		
	set_alarm(rtc_tm->tm_hour,rtc_tm->tm_min,rtc_tm->tm_sec);

}

void alarm_thread(void* parameter)
{
	set_next_alarm(10);
	while(1)	
	{		
		if(rt_sem_take(&(alarm_sem), RT_WAITING_FOREVER) != RT_EOK) continue;
		//if(!sync)
		//{
		//	sync=RT_TRUE;
			if(!server_time_got)
			sync_server(dev_cap,0);
			//sync_server(dev_cap,1);
		//	sync=RT_FALSE;
		//}
		set_next_alarm(10);
	}

}
static rt_err_t gprs_rx_ind(rt_device_t dev, rt_size_t size)
{	
	rt_sem_release(&(server_sem));
	return RT_EOK;
}

//uart1 for debug
//uart2 for wifi
//uart3 for cap
//uart4 for lcd
int init_cap()
{
	sram_init();	
	set_date(2015,10,24);
	set_time(14,44,30);
	//rt_mutex_init(&lock, "lock", RT_IPC_FLAG_PRIO);
	server_time=(char *)sram_malloc(13);
	wifi_result=(char *)sram_malloc(512);
	rt_memset(wifi_result,0,512);
	rt_sem_init(&(alarm_sem), "alarm", 0, 0);
	rt_thread_startup(rt_thread_create("alarm",alarm_thread, 0,1024, 20, 10));
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
		rt_thread_startup(rt_thread_create("thread_lcd",lcd_thread, 0,256, 20, 10));
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
		rt_sem_init(&(wifi_rx_sem), "wifi_rx", 0, 0);
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
		strcpy(cmd,"AT+HTPFT=ON\n");
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
		strcpy(cmd,"AT+HTPURL=/saveData/airmessage/messMgr.do\n");
		rt_device_write(dev_wifi, 0, (void *)cmd, rt_strlen(cmd));
		rt_thread_delay(10);
		rt_memset(cmd,0,512);
		strcpy(cmd,"AT+Z\n");
		rt_device_write(dev_wifi, 0, (void *)cmd, rt_strlen(cmd));
		rt_thread_delay(300);
		while(rt_device_read(dev_wifi, 0, &done, 1)==1);
		#if 0
		rt_memset(cmd,0,512);
		//strcpy(cmd,"JSONStr={\"0\":\"2\",\"30\":\"1234abcd\",\"35\":\"192.1.3.21\",\"36\":\"8769\",\"60\":\"34\",\"61\":\"54\",\"62\":\"14\",\"63\":\"54\",\"64\":\"120\",\"65\":\"121\",\"90\":\"92\",\"103\":\"2015-10-20 12:16\"}");
		strcpy(cmd,"JSONStr={\"0\":\"5\",\"30\":\"230FFEE9981283737D\",\"35\":\"192.1.3.55\",\"36\":\"8769\"}");
		rt_device_write(dev_wifi, 0, (void *)cmd, rt_strlen(cmd));
		rt_thread_delay(10);
		rt_memset(cmd,0,512);
		strcpy(cmd,"JSONStr={\"0\":\"2\",\"30\":\"1234abcd\",\"35\":\"192.1.3.21\",\"36\":\"8769\",\"60\":\"34\",\"61\":\"54\",\"62\":\"14\",\"63\":\"54\",\"64\":\"120\",\"65\":\"121\",\"90\":\"92\",\"103\":\"2015-10-20 12:16\"}");
		rt_device_write(dev_wifi, 0, (void *)cmd, rt_strlen(cmd));
		rt_thread_delay(10);
		rt_memset(cmd,0,512);
		strcpy(cmd,"JSONStr={\"0\":\"6\",\"30\":\"230FFEE9981283737D\",\"35\":\"192.1.3.55\",\"36\":\"8769\"}");
		rt_device_write(dev_wifi, 0, (void *)cmd, rt_strlen(cmd));
		rt_thread_delay(10);
		rt_device_write(dev_wifi, 0, (void *)cmd, rt_strlen(cmd));
		rt_sem_take(&(server_sem), RT_WAITING_FOREVER);
		rt_thread_delay(10);
		#endif
		sram_free(cmd);
		rt_device_set_rx_indicate(dev_wifi, wifi_rx_ind);
		//rt_thread_startup(rt_thread_create("thread_wifi",wifi_thread, 0,512, 20, 10));
	}
	else
	{
		rt_kprintf("open youren wifi uart2 failed\r\n");
		return -1;
	}
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
		#if 0
		server_time[0]=0x6c;server_time[1]=ARM_TO_CAP;
		server_time[2]=0x00;server_time[3]=0x01;server_time[4]=0x06;
		server_time[5]=0x0f;server_time[6]=0x0a;
		server_time[7]=0x18;server_time[8]=0x0e;
		server_time[9]=0x0e;server_time[10]=0x18;
		int crc=CRC_check(server_time,11);
		server_time[11]=(crc&0xff00)>>8;server_time[12]=crc&0x00ff;
		rt_device_write(dev_cap,0,server_time,13);
		#endif
		rt_sem_init(&(cap_rx_sem), "cap_rx", 0, 0);
		rt_device_set_rx_indicate(dev_cap, cap_rx_ind);
		rt_thread_startup(rt_thread_create("thread_cap",cap_thread, 0,2048, 20, 10));
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
	#if 1
	dev_gprs=rt_device_find("uart5");
	if (rt_device_open(dev_gprs, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) == RT_EOK)			
	{		
		unsigned char switch_at='+';
		unsigned char done='a';
		char *cmd;
		rt_sem_init(&(server_sem), "server_rx", 0, 0);
		rt_device_set_rx_indicate(dev_gprs, gprs_rx_ind);
		//rt_thread_startup(rt_thread_create("thread_wifi",wifi_thread, 0,512, 20, 10));
		#if 0
		rt_thread_delay(200);	
		rt_device_write(dev_gprs, 0, (void *)&switch_at, 1);
		rt_thread_delay(1);
		rt_device_write(dev_gprs, 0, (void *)&switch_at, 1);
		rt_thread_delay(1);
		rt_device_write(dev_gprs, 0, (void *)&switch_at, 1);
		rt_thread_delay(1);
		rt_device_write(dev_gprs, 0, (void *)&done, 1);
		rt_thread_delay(10);
		cmd=(char *)sram_malloc(512);
		rt_memset(cmd,0,512);
		strcpy(cmd,"AT+CIPSCONT=1,\"TCP\",\"101.200.182.92\", 8080,1\n");
		rt_device_write(dev_gprs, 0, (void *)cmd, rt_strlen(cmd));
		rt_thread_delay(10);
		rt_memset(cmd,0,512);
		strcpy(cmd,"AT+CIMOD=\"3\"\n");
		rt_device_write(dev_gprs, 0, (void *)cmd, rt_strlen(cmd));
		rt_thread_delay(10);
		rt_memset(cmd,0,512);
		strcpy(cmd,"AT+CSTT=\"UNINET\"\n");
		rt_device_write(dev_gprs, 0, (void *)cmd, rt_strlen(cmd));
		rt_thread_delay(10);
		rt_memset(cmd,0,512);
		strcpy(cmd,"ATW\n");
		rt_device_write(dev_gprs, 0, (void *)cmd, rt_strlen(cmd));
		rt_thread_delay(10);
		rt_memset(cmd,0,512);
		//strcpy(cmd,"AT+ENTM\n");
		//rt_device_write(dev_gprs, 0, (void *)cmd, rt_strlen(cmd));
		//rt_thread_delay(100);
		sram_free(cmd);
		#endif
		#if 0
		char *cmd=(char *)sram_malloc(512);
		rt_memset(cmd,0,512);
		strcpy(cmd,"AT+CIPCFG=1,0,0,50,0,0\n");
		rt_device_write(dev_gprs, 0, (void *)cmd, rt_strlen(cmd));
		rt_thread_delay(10);
		rt_memset(cmd,0,512);
		strcpy(cmd,"AT+CIPPACK=0,"",0\n");
		rt_device_write(dev_gprs, 0, (void *)cmd, rt_strlen(cmd));
		rt_thread_delay(10);
		rt_memset(cmd,0,512);
		strcpy(cmd,"AT+CIPPACK=1,"",0\n");
		rt_device_write(dev_gprs, 0, (void *)cmd, rt_strlen(cmd));
		rt_thread_delay(10);
		rt_memset(cmd,0,512);
		strcpy(cmd,"AT+CIPSCONT=1,\"TCP\",\"101.200.182.92\", 8080,1\n");
		rt_device_write(dev_gprs, 0, (void *)cmd, rt_strlen(cmd));
		rt_thread_delay(10);
		rt_memset(cmd,0,512);
		strcpy(cmd,"AT+CIMOD=\"3\"\n");
		rt_device_write(dev_gprs, 0, (void *)cmd, rt_strlen(cmd));
		rt_thread_delay(10);
		rt_memset(cmd,0,512);
		strcpy(cmd,"AT+CSTT=\"UNINET\"\n");
		rt_device_write(dev_gprs, 0, (void *)cmd, rt_strlen(cmd));
		rt_thread_delay(10);
		rt_memset(cmd,0,512);
		cmd=(char *)sram_malloc(512);
		rt_memset(cmd,0,512);
		strcpy(cmd,"AT+ICF=3,3\n");
		rt_device_write(dev_gprs, 0, (void *)cmd, rt_strlen(cmd));
		rt_thread_delay(10);
		cmd=(char *)sram_malloc(512);
		rt_memset(cmd,0,512);
		strcpy(cmd,"AT+CIPR=115200\n");
		rt_device_write(dev_gprs, 0, (void *)cmd, rt_strlen(cmd));
		rt_thread_delay(10);
		cmd=(char *)sram_malloc(512);
		rt_memset(cmd,0,512);
		strcpy(cmd,"ATW\n");
		rt_device_write(dev_gprs, 0, (void *)cmd, rt_strlen(cmd));
		rt_thread_delay(10);
		cmd=(char *)sram_malloc(512);
		rt_memset(cmd,0,512);
		strcpy(cmd,"AT+CIRESET\n");
		rt_device_write(dev_gprs, 0, (void *)cmd, rt_strlen(cmd));
		rt_thread_delay(300);
		#endif
	}
	#endif
	return 0;
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
	rt_device_write(dev_gprs, 0, (void *)arg, rt_strlen(arg));
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

