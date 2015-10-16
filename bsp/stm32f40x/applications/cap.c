#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <stdlib.h>
#include <string.h>

#ifdef  RT_USING_COMPONENTS_INIT
#include <components.h>
#endif  /* RT_USING_COMPONENTS_INIT */
#include "cJSON.h"
#include "cap.h"
#define SUB_PROCESS "[CapProcess]"
struct rt_semaphore cap_rx_sem,wifi_rx_sem,server_sem,lcd_rx_sem;
rt_device_t dev_cap,dev_wifi,dev_lcd;
int data_co2=0,data_ch2o;
char *post_message=NULL,can_send=0;
char *server_time;
char wifi_result[512]={0};
char *http_parse_result(const char*lpbuf);
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
	response = (char *)malloc(rt_strlen(ptmp)+1);  
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
	out=cJSON_Print(root);	
	cJSON_Delete(root);
	if(old)
		free(old);
	
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
	free(pad);
	return out;
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
				out=(char *)malloc(nLen+1);		
				rt_memset(out,'\0',nLen+1);	
				memcpy(out,data->valuestring,nLen);	
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

	char httpd_send[64]={0};//"AT+HTTPDT\r\n";
	char httpd_local[64]={0};//"AT+HTTPPH=/mango/checkDataYes\r\n";
	strcpy(httpd_local,"AT+HTTPPH=/saveData/airmessage/messMgr.do?JSONStr=");
	strcpy(httpd_send,"AT+HTTPDT\n");	
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
									rt_sprintf(date,"20%02d-%02d-%02d%%20%02d:%02d",to_check[i+5],to_check[i+6],to_check[i+7],to_check[i+8],to_check[i+9],to_check[i+10]);
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
									rt_device_read(dev_cap,0,server_time,13);
								}
								break;
							default:
								{
									/*get cap data*/
									if(to_check[i+5]==0x45 && to_check[i+6]==0x52 && to_check[i+7]==0x52 && to_check[i+8]==0x4f && to_check[i+9]==0x52)
									{
										rt_sprintf(error,"%dth%%20sensor%%20possible%%20error",to_check[i+3]);
										post_message=add_item(post_message,ID_ALERT_CAP_FAILED,error);
									}
									else
									{
										rt_sprintf(id,"%d",message_type);
										rt_sprintf(data,"%d%d",to_check[i+5],to_check[i+6]);
										//rt_kprintf("pre data %s %d\r\n",data,rt_strlen(data));
										
										if(to_check[i+7]>=rt_strlen(data))
										{									
											rt_sprintf(data,"0.%d%d",to_check[i+5],to_check[i+6]);
										}
										else if(to_check[i+7]==0)
										{									
											rt_sprintf(data,"%d%d.0",to_check[i+5],to_check[i+6]);
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
										rt_kprintf(SUB_PROCESS"id %s data %s\r\n",id,data);
										post_message=add_item(post_message,id,data);
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
						j=0;
						for(i=0;i<rt_strlen(post_message);i++)
						{
							if(post_message[i]=='\n'||post_message[i]=='\r'||post_message[i]=='\t')
								j++;
						}
						rt_kprintf(SUB_PROCESS"send post_message %s",post_message);
						char *out1=malloc(rt_strlen(post_message)-j+1);
						rt_memset(out1,'\0',rt_strlen(post_message)-j+1);
						j=0;
						for(i=0;i<rt_strlen(post_message);i++)
						{
							if(post_message[i]!='\r'&&post_message[i]!='\n'&&post_message[i]!='\t')		
							{
								out1[j++]=post_message[i];
							}
						}
						//save_to_file(date,out1);
						rt_kprintf(SUB_PROCESS"send web %s",out1);
						//rcv=send_web(URL,out1,9);
						char *send=(char *)malloc(rt_strlen(out1)+rt_strlen(httpd_local)+1+1);
						rt_memset(send,'\0',rt_strlen(out1)+rt_strlen(httpd_local)+1+1);
						strcpy(send,httpd_local);
						strcat(send,out1);
						strcat(send,"\n");
						rt_kprintf("send %s",send);
						rt_device_write(dev_wifi, 0, (void *)send, rt_strlen(send));
						rt_thread_delay(10);
						rt_device_write(dev_wifi, 0, (void *)httpd_send, rt_strlen(httpd_send));
						rt_free(send);
						rt_free(out1);
						rt_sem_take(&(server_sem), RT_WAITING_FOREVER);//wait for server respond
						free(post_message);
						post_message=NULL;
						free(out1);
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
	char *ptr=rt_malloc(32);			
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
	unsigned char switch_at='+';
	unsigned char done='a';
	int len=0,i=0;
	char *ptr=(char *)malloc(256);
	char httpd_url[64]={0};//"AT+HTTPURL=http://101.200.182.92,8080\r\n";
	char httpd_mode[64]={0};//"AT+HTTPTP=GET\r\n";	
	strcpy(httpd_url,"AT+HTTPURL=http://101.200.182.92:8080\n");
	strcpy(httpd_mode,"AT+HTTPTP=GET\n");
	//rt_sem_init(&(wifi_rx_sem), "wifi_rx", 0, 0);
	rt_sem_init(&(server_sem), "server_rx", 0, 0);
	//rt_device_set_rx_indicate(dev_wifi, wifi_rx_ind);
	//rt_thread_startup(rt_thread_create("thread_wifi",wifi_rcv, 0,512, 20, 10));
	rt_thread_delay(200);	
	rt_device_write(dev_wifi, 0, (void *)&switch_at, 1);
	rt_thread_delay(3);
	rt_device_write(dev_wifi, 0, (void *)&switch_at, 1);
	rt_thread_delay(3);
	rt_device_write(dev_wifi, 0, (void *)&switch_at, 1);
	rt_thread_delay(3);
	rt_device_write(dev_wifi, 0, (void *)&done, 1);
	rt_thread_delay(1);
	rt_device_write(dev_wifi, 0, (void *)httpd_url, rt_strlen(httpd_url));
	rt_thread_delay(30);	
	rt_device_write(dev_wifi, 0, (void *)httpd_url, rt_strlen(httpd_url));
	rt_thread_delay(30);
	rt_device_write(dev_wifi, 0, (void *)httpd_mode, rt_strlen(httpd_mode));
	rt_thread_delay(30);
	while(1)	
	{		
		if (rt_sem_take(&(wifi_rx_sem), RT_WAITING_FOREVER) != RT_EOK) continue;	
		len=rt_device_read(dev_wifi, 0, ptr+i, 128);
		if(len>0)
		{
			int m;
			for(m=0;m<len;m++)
				rt_kprintf("%c",ptr[i+m]);
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
						rt_free(id);
					}
					if(start!=RT_NULL)
					{
						rt_kprintf("start time %s\n",start);
						rt_free(start);
					}
					if(stop!=RT_NULL)
					{
						rt_kprintf("stop time %s\n",stop);
						rt_free(stop);
					}
					rt_free(result);
				}
				#endif
			}		
			rt_memset(ptr,'\0',256);
			i=0;		
			rt_sem_release(&(server_sem));    
		}
	}	
}

//uart1 for debug
//uart2 for wifi
//uart3 for cap
//uart4 for lcd
int init_cap()
{
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
		rt_thread_startup(rt_thread_create("thread_cap",cap_thread, 0,512, 20, 10));
		//rt_device_write(dev_cap, 0, (void *)read_co2, 9);
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
		
		rt_sem_init(&(wifi_rx_sem), "wifi_rx", 0, 0);
		rt_device_set_rx_indicate(dev_wifi, wifi_rx_ind);
		rt_thread_startup(rt_thread_create("thread_wifi",wifi_thread, 0,512, 20, 10));
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
		char *out1=malloc(rt_strlen(post_message)+3*j+1);
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
		free(post_message);
		char *send=(char *)malloc(rt_strlen(out1)+rt_strlen(httpd_local)+1+1);
		rt_memset(send,'\0',rt_strlen(out1)+rt_strlen(httpd_local)+1+1);
		strcpy(send,httpd_local);
		strcat(send,out1);
		strcat(send,"\n");
		rt_kprintf("send %s",send);
		rt_device_write(dev_wifi, 0, (void *)send, rt_strlen(send));
		rt_thread_delay(10);
		rt_device_write(dev_wifi, 0, (void *)httpd_send, rt_strlen(httpd_send));
		rt_free(send);
		rt_free(out1);
		//rt_sem_take(&(server_sem), RT_WAITING_FOREVER);
		rt_thread_delay(600);
		}
#endif
}
#ifdef RT_USING_FINSH
#include <finsh.h>

void wifi(char *arg)
{
	char *cmd=(char *)rt_malloc(strlen(arg)+3);
	rt_memset(cmd,0,strlen(arg)+2);
	strcpy(cmd,arg);
	strcat(cmd,"\n");
	rt_kprintf("%s",cmd);
	rt_device_write(dev_wifi, 0, (void *)cmd, rt_strlen(cmd));
	rt_free(cmd);
}
void rst()
{
	NVIC_SystemReset();
}
FINSH_FUNCTION_EXPORT(wifi, wifi cmd)
FINSH_FUNCTION_EXPORT(rst, system reset)
#endif

