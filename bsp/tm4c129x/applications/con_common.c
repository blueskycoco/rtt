#include "con_socket.h"
#include "con_uart.h"
#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include "board.h"

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/common.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#if 0
void IntGpioD()
{
	if(MAP_GPIOIntStatus(GPIO_PORTD_BASE, true)&GPIO_PIN_2)
	{			
		MAP_GPIOIntClear(GPIO_PORTD_BASE, GPIO_PIN_2);
		if(MAP_GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_2)!=GPIO_PIN_2)
			MAP_GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, 0);
		else
			MAP_GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, GPIO_PIN_4);
		//ind[3]=((MAP_GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_2)&(GPIO_PIN_2))==0)?RT_TRUE:RT_FALSE;
		rt_kprintf("gpiod 2 int \r\n");
	}	
}

#else
unsigned char config_local_ip[4+7]				={0xF5,0x8A,0x00,0xff,0xff,0xff,0xff,0x26,0xfa,0x00,0x00};/*local ip*/
unsigned char config_socket0_local_port[2+7]	={0xF5,0x8A,0x01,0xff,0xff,0x26,0xfa,0x00,0x00};/*local port0*/
unsigned char config_socket1_local_port[2+7]	={0xF5,0x8A,0x02,0xff,0xff,0x26,0xfa,0x00,0x00};/*local port1*/
unsigned char config_socket2_local_port[2+7]	={0xF5,0x8A,0x03,0xff,0xff,0x26,0xfa,0x00,0x00};/*local port2*/
unsigned char config_socket3_local_port[2+7]	={0xF5,0x8A,0x04,0xff,0xff,0x26,0xfa,0x00,0x00};/*local port3*/
unsigned char config_sub_msk[4+7]				={0xF5,0x8A,0x05,0xff,0xff,0xff,0xff,0x26,0xfa,0x00,0x00};/*sub msk*/
unsigned char config_gw[4+7]					={0xF5,0x8A,0x06,0xff,0xff,0xff,0xff,0x26,0xfa,0x00,0x00};/*gw*/
unsigned char config_mac[6+7]					={0xF5,0x8A,0x07,0xff,0xff,0xff,0xff,0xff,0xff,0x26,0xfa,0x00,0x00};/*mac*/
unsigned char config_socket0_ip[4+7]			={0xF5,0x8A,0x08,0xff,0xff,0xff,0xff,0x26,0xfa,0x00,0x00};/*socket 0 ip*/
unsigned char config_socket1_ip[4+7]			={0xF5,0x8A,0x09,0xff,0xff,0xff,0xff,0x26,0xfa,0x00,0x00};/*socket 1 ip*/
unsigned char config_socket2_ip[4+7]			={0xF5,0x8A,0x0a,0xff,0xff,0xff,0xff,0x26,0xfa,0x00,0x00};/*socket 2 ip*/
unsigned char config_socket3_ip[4+7]			={0xF5,0x8A,0x0b,0xff,0xff,0xff,0xff,0x26,0xfa,0x00,0x00};/*socket 3 ip*/
unsigned char config_socket0_ip6[64+7]			={0xF5,0x8A,0x0c,0xff,0xff,0xff,0xff,0x26,0xfa,0x00,0x00};/*socket 0 ip6*/
unsigned char config_socket1_ip6[64+7]			={0xF5,0x8A,0x0d,0xff,0xff,0xff,0xff,0x26,0xfa,0x00,0x00};/*socket 1 ip6*/
unsigned char config_socket2_ip6[64+7]			={0xF5,0x8A,0x0e,0xff,0xff,0xff,0xff,0x26,0xfa,0x00,0x00};/*socket 2 ip6*/
unsigned char config_socket3_ip6[64+7]			={0xF5,0x8A,0x0f,0xff,0xff,0xff,0xff,0x26,0xfa,0x00,0x00};/*socket 3 ip6*/
unsigned char config_socket0_remote_port[2+7]	={0xF5,0x8A,0x10,0xff,0xff,0x26,0xfa,0x00,0x00};/*socket 0 port*/
unsigned char config_socket1_remote_port[2+7]	={0xF5,0x8A,0x11,0xff,0xff,0x26,0xfa,0x00,0x00};/*socket 1 port*/
unsigned char config_socket2_remote_port[2+7]	={0xF5,0x8A,0x12,0xff,0xff,0x26,0xfa,0x00,0x00};/*socket 2 port*/
unsigned char config_socket3_remote_port[2+7]	={0xF5,0x8A,0x13,0xff,0xff,0x26,0xfa,0x00,0x00};/*socket 3 port*/
unsigned char config_net_protol0[1+7]			={0xF5,0x8A,0x14,0xff,0x26,0xfa,0x00,0x00};/*protol0*/
unsigned char config_net_protol1[1+7]			={0xF5,0x8A,0x15,0xff,0x26,0xfa,0x00,0x00};/*protol1*/
unsigned char config_net_protol2[1+7]			={0xF5,0x8A,0x16,0xff,0x26,0xfa,0x00,0x00};/*protol2*/
unsigned char config_net_protol3[1+7]			={0xF5,0x8A,0x17,0xff,0x26,0xfa,0x00,0x00};/*protol3*/
unsigned char config_socket_mode0[1+7]			={0xF5,0x8A,0x18,0xff,0x26,0xfa,0x00,0x00};/*server mode0*/
unsigned char config_socket_mode1[1+7]			={0xF5,0x8A,0x19,0xff,0x26,0xfa,0x00,0x00};/*server mode1*/
unsigned char config_socket_mode2[1+7]			={0xF5,0x8A,0x1a,0xff,0x26,0xfa,0x00,0x00};/*server mode2*/
unsigned char config_socket_mode3[1+7]			={0xF5,0x8A,0x1b,0xff,0x26,0xfa,0x00,0x00};/*server mode3*/
unsigned char config_uart_baud0[1+7]			={0xF5,0x8A,0x1c,0xff,0x26,0xfa,0x00,0x00};/*uart baud*/
unsigned char config_uart_baud1[1+7]			={0xF5,0x8A,0x1d,0xff,0x26,0xfa,0x00,0x00};/*uart baud*/
unsigned char config_uart_baud2[1+7]			={0xF5,0x8A,0x1e,0xff,0x26,0xfa,0x00,0x00};/*uart baud*/
unsigned char config_uart_baud3[1+7]			={0xF5,0x8A,0x1f,0xff,0x26,0xfa,0x00,0x00};/*uart baud*/
unsigned char get_config[35];//			={0xF5,0x8B,0x0f,0xff,0xff,0xff,0xff,0x27,0xfa,0x00,0x00};/*get config 0xf5,0x8b,********0x27,0xfa,0x00,0x00*/
rt_thread_t tid_common_w[4]={RT_NULL,RT_NULL,RT_NULL,RT_NULL},tid_common_r[4]={RT_NULL,RT_NULL,RT_NULL,RT_NULL};
rt_device_t common_dev[4] = {RT_NULL,RT_NULL,RT_NULL,RT_NULL};
bool ind[4]={RT_FALSE,RT_FALSE,RT_FALSE,RT_FALSE};
enum STATE_OP{
	GET_F5,
	GET_8A_8B,
	GET_DATA,
	GET_26,
	GET_FA,
	GET_CHECSUM
};
struct rt_semaphore rx_sem[4];
//rt_mutex_t mutex = RT_NULL;
void common_thread_entry(void* parameter);
//struct rt_thread common_thread[4];
//ALLIGN(RT_ALIGN_SIZE)
//static char common_thread_stack[4][2048];
int which_common_dev(rt_device_t *dev,rt_device_t dev2)
{
	int i=0;
	for(i=0;i<4;i++)
		if(dev[i]==dev2)
		{
			//DBG("Uart %d 's setting\r\n",i);
			break;
		}
	return i;
}
void IntGpioD()
{
	if(MAP_GPIOIntStatus(GPIO_PORTD_BASE, true)&GPIO_PIN_2)
	{
		MAP_GPIOIntClear(GPIO_PORTD_BASE, GPIO_PIN_2);
		ind[3]=((MAP_GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_2)&(GPIO_PIN_2))==0)?RT_TRUE:RT_FALSE;
		rt_kprintf("gpiod 2 int %d\r\n",ind[3]);
	}	
}/*
void IntGpioB()
{
	if(MAP_GPIOIntStatus(GPIO_PORTB_BASE, true)&GPIO_PIN_4)
	{
		MAP_GPIOIntClear(GPIO_PORTB_BASE, GPIO_PIN_4);
		ind[3]=((MAP_GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_4)&(GPIO_PIN_4))==0)?RT_TRUE:RT_FALSE;
		rt_kprintf("gpiob 4 int %d\r\n",ind[3]);
	}	
}
*/
/*get config data to global config zone, or get socket data to buffer*/

void common_rw_config(rt_device_t dev)
{
	static rt_uint8_t buf[6];
	rt_uint8_t i=0;
	static int data_len,crc_len;
	static unsigned char crc[2];
	char ch;	
	static rt_uint8_t len=0,param;
	static enum STATE_OP state=GET_F5;
	DBG("enter common_rw_config\r\n");
	while((rt_device_read(dev, 0, &ch, 1) == 1))
	{
		DBG("%x ==> %d\r\n",ch,state);
		switch(state)
		{
			case GET_F5:
			{
				if(ch==0xf5)
				{
					DBG("Dev %d , 0XF5 Got\r\n",which_common_dev(common_dev,dev));
					state=GET_8A_8B;
				}
			}
			break;
			case GET_8A_8B:
			{
				if(ch==0x8a)
				{
					DBG("Dev %d , 0X8A Got\r\n",which_common_dev(common_dev,dev));
					state=GET_DATA;
					data_len=0;
				}
				else if(ch==0x8b)
				{
					/*send config data out*/
					unsigned char i,*ptr,*ptr1;
					int result=0;
					int devices=which_common_dev(common_dev,dev);
					DBG("Dev %d , 0X8B Got\r\n",devices);
					get_config[0]=0xf5;
					get_config[1]=0x8C;
					result=0xf5+0x8c+0x27+0xfa;
					for(i=0;i<4;i++)
						get_config[2+i]=(g_conf.local_ip>>(i*8))&0xff;
					for(i=0;i<2;i++)
						get_config[6+i]=g_conf.local_port[i];					
					for(i=0;i<4;i++)
						get_config[8+i]=g_conf.sub_msk[i];					
					for(i=0;i<4;i++)
						get_config[12+i]=g_conf.gw[i];
					for(i=0;i<6;i++)
						get_config[16+i]=g_conf.mac[i];
					switch(devices)
					{
						case 0:
							ptr=g_conf.remote_ip0;
							ptr1=g_conf.remote_port0;
							break;
						case 1:
							ptr=g_conf.remote_ip1;
							ptr1=g_conf.remote_port1;
							break;
						case 2:
							ptr=g_conf.remote_ip2;
							ptr1=g_conf.remote_port2;
							break;
						case 3:
							ptr=g_conf.remote_ip3;
							ptr1=g_conf.remote_port3;
							break;
													
					}
					
					for(i=0;i<4;i++)
						get_config[22+i]=ptr[i];					
					for(i=0;i<2;i++)
						get_config[26+i]=ptr1[i];
					get_config[28]=g_conf.protol[devices];
					get_config[29]=g_conf.server_mode[devices];
					get_config[30]=g_conf.common_baud[devices];					
					get_config[31]=0x27;
					get_config[32]=0xfa;
					for(i=2;i<31;i++)
						result=result+get_config[i];
					get_config[33]=(result&0xff00)>>8;
					get_config[34]=result&0xff;
					rt_device_write(dev,0,get_config,sizeof(get_config));
					for(i=0;i<sizeof(get_config);i++)
						DBG("==>%x \r\n",get_config[i]);
					state=GET_F5;
					print_config();
				}
				
			}
			break;
			case GET_DATA:
			{
				if(data_len==0)
				{
					param=ch;
					DBG("Dev %d , Command 0x%2x Got\r\n",which_common_dev(common_dev,dev),ch);
					if(ch==0||ch==2||ch==3||ch==5||ch==6||ch==7||ch==8)
						len=4;
					else if(ch==1||ch==9||ch==10||ch==11||ch==12)
						len=2;
					else if(ch==4)
						len=6;
					else if(ch==13||ch==14||ch==15)
						len=1;
				}
				else
					buf[data_len-1]=ch;

				if(data_len==len)
					state=GET_26;
				else
				{
					state=GET_DATA;
					data_len++;
				}
			}
			break;
			case GET_26:
			{
				if(ch==0x26)
				{
					DBG("Dev %d , 0X26 Got\r\n",which_common_dev(common_dev,dev));
					state=GET_FA;
				}
				else
					state=GET_F5;
			}
			break;
			case GET_FA:
			{
				if(ch==0xfa)
				{
					state=GET_CHECSUM;
					crc_len=0;
				}
				else
					state=GET_F5;
			}
			break;
			case GET_CHECSUM:
			{
				crc[crc_len]=ch;
				if(crc_len!=1)
					crc_len++;
				else
				{
					unsigned char *ptr;
					DBG("Dev %d , 0XFA Got\r\n",which_common_dev(common_dev,dev));
					int result=0xf5+0x8a+param+0x26+0xfa;
					for(i=0;i<len;i++)
						result=result+buf[i];
					if(result==(crc[0]<<8|crc[1]))
					{
						switch(param)
						{
							case 0://local ip		
								ptr=g_conf.local_ip;
							break;
							case 1://local port 	
								ptr=g_conf.local_port;
							break;
							case 2://sub msk		
								ptr=g_conf.sub_msk;
							break;
							case 3://gw ip		
								ptr=g_conf.gw;
							break;
							case 4://mac		
								ptr=g_conf.mac;
								DBG("to write mac\r\n");
							break;
							case 5://socket 0 ip		
							{
								if(which_common_dev(common_dev,dev)==0)
									ptr=g_conf.remote_ip0;	
								else if(which_common_dev(common_dev,dev)==1)
									ptr=g_conf.remote_ip1;
								else if(which_common_dev(common_dev,dev)==2)
									ptr=g_conf.remote_ip2;
								else
								{
									ptr=g_conf.remote_ip3;
									DBG("to write socket3 ip\r\n");
								}
							}
							break;
							case 9://socket 0 port										
							{
								if(which_common_dev(common_dev,dev)==0)
									ptr=g_conf.remote_port0;
								else if(which_common_dev(common_dev,dev)==1)
									ptr=g_conf.remote_port1;
								else if(which_common_dev(common_dev,dev)==2)
									ptr=g_conf.remote_port2;
								else
								{
									ptr=g_conf.remote_port3;
									DBG("to write socket3 port\r\n");
								}
							}
							break;									
							case 13://protol	
								i=which_common_dev(common_dev,dev);
								g_conf.protol[i]=buf[0];
							break;
							case 14://server or client mode
								i=which_common_dev(common_dev,dev);
								g_conf.server_mode[i]=buf[0];
							break;									
							case 15://socket common baud
							{

								struct serial_configure config;
								ptr=g_conf.common_baud+which_common_dev(common_dev,dev);
								i=which_common_dev(common_dev,dev);
								if(buf[0]==0)
									config.baud_rate = 115200;
								else if(buf[0]==1)
									config.baud_rate = 406800;
								else if(buf[0]==2)
									config.baud_rate = 921600;
								else if(buf[0]==3)
									config.baud_rate = 2000000;
								else if(buf[0]==4)
									config.baud_rate = 4000000;
								else if(buf[0]==5)
									config.baud_rate = 6000000;

								config.bit_order = BIT_ORDER_LSB;
								config.data_bits = DATA_BITS_8;
								config.parity	 = PARITY_NONE;
								config.stop_bits = STOP_BITS_1;
								config.invert	 = NRZ_NORMAL;
								config.bufsz	 = RT_SERIAL_RB_BUFSZ;
								rt_device_control(dev,RT_DEVICE_CTRL_CONFIG,&config);								
								g_conf.common_baud[i]=buf[0];
								//rt_kprintf("set common %d buf[%d] %d to %x baud\r\n",i,i,buf[0],config.baud_rate);
							}
							break;
							default:
								ptr=NULL;
						}
					if(ptr!=NULL && param!=13 &&param!=14 &&param!=15)
						for(i=0;i<len;i++)
						{
							ptr[i]=buf[i];
							DBG("set %d %d\r\n",ptr[i],buf[i]);
						}
					if(param==4)
						DBG("mac %x %x %x %x %x %x \r\n",g_conf.mac[0],g_conf.mac[1],g_conf.mac[2],g_conf.mac[3],g_conf.mac[4],g_conf.mac[5]);
				}
					else
						DBG("Dev %d , crc fault %x!=%x\r\n",which_common_dev(common_dev,dev),result,(crc[0]<<8)|crc[1]);
				state=GET_F5;
				}
			
			}
			break;
			default:
			{
				DBG("Invaild %x\r\n",ch);
				state=GET_F5;
			}
			break;
		}
	}
}
/*rt_bool_t ind_low(rt_device_t dev)
{
	if(which_common_dev(common_dev,dev)==0)
		return (((MAP_GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_4)&(0x1<<GPIO_PIN_4))==0)?RT_TRUE:RT_FALSE);
	else if(which_common_dev(common_dev,dev)==1)
		return (((MAP_GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1)&(0x1<<GPIO_PIN_1))==0)?RT_TRUE:RT_FALSE);
	else if(which_common_dev(common_dev,dev)==2)
		return (((MAP_GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2)&(0x1<<GPIO_PIN_2))==0)?RT_TRUE:RT_FALSE);
	else if(which_common_dev(common_dev,dev)==3)
	{
		DBG("IND %x\r\n",MAP_GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_2));
		return (((MAP_GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_2)&(GPIO_PIN_2))==0)?RT_TRUE:RT_FALSE);
	}

	return RT_FALSE;
}
*/
void cnn_out(int index,int level)
{
	switch(index)
		{
		case 0:
			if(level)
				MAP_GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_4,GPIO_PIN_4);
			else
				MAP_GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_4,0);	
			break;
		case 1:
			if(level)
				MAP_GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_5,GPIO_PIN_5);
			else
				MAP_GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_5,0);	
			break;
		case 2:
			if(level)
				MAP_GPIOPinWrite(GPIO_PORTK_BASE,GPIO_PIN_2,GPIO_PIN_2);
			else
				MAP_GPIOPinWrite(GPIO_PORTK_BASE,GPIO_PIN_2,0);	
			break;
		case 3:
			if(level)
				MAP_GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_4,GPIO_PIN_4);
			else
				MAP_GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_4,0);	
			break;
		default:
			break;
		}
}
int baud(int type)
{
	switch(type)
		{
		case 0:
			return 115200;
		case 1:
			return 460800;
		case 2:
			return 921600;
		case 3:
			return 1000000;
		case 4:
			return 2000000;
		case 5:
			return 4000000;
		case 6:
			return 6000000;
		}
	return 0;
}
void print_config()
{
	rt_kprintf("local_ip %d.%d.%d.%d\r\n",g_conf.local_ip[0],g_conf.local_ip[1],g_conf.local_ip[2],g_conf.local_ip[3]);
	rt_kprintf("local_port %d\r\n",g_conf.local_port[0]<<8|g_conf.local_port[1]);
	rt_kprintf("sub_msk %d.%d.%d.%d\r\n",g_conf.sub_msk[0],g_conf.sub_msk[1],g_conf.sub_msk[2],g_conf.sub_msk[3]);
	rt_kprintf("gw %d.%d.%d.%d\r\n",g_conf.gw[0],g_conf.gw[1],g_conf.gw[2],g_conf.gw[3]);
	rt_kprintf("mac %x:%x:%x:%x:%x:%x\r\n",g_conf.mac[0],g_conf.mac[1],g_conf.mac[2],g_conf.mac[3],g_conf.mac[4],g_conf.mac[5]);
	rt_kprintf("remote_ip0 %d.%d.%d.%d\r\n",g_conf.remote_ip0[0],g_conf.remote_ip0[1],g_conf.remote_ip0[2],g_conf.remote_ip0[3]);
	rt_kprintf("remote_ip1 %d.%d.%d.%d\r\n",g_conf.remote_ip1[0],g_conf.remote_ip1[1],g_conf.remote_ip1[2],g_conf.remote_ip1[3]);
	rt_kprintf("remote_ip2 %d.%d.%d.%d\r\n",g_conf.remote_ip2[0],g_conf.remote_ip2[1],g_conf.remote_ip2[2],g_conf.remote_ip2[3]);
	rt_kprintf("remote_ip3 %d.%d.%d.%d\r\n",g_conf.remote_ip3[0],g_conf.remote_ip3[1],g_conf.remote_ip3[2],g_conf.remote_ip3[3]);
	rt_kprintf("remote_port0 %d\r\n",g_conf.remote_port0[0]<<8|g_conf.remote_port0[1]);
	rt_kprintf("remote_port1 %d\r\n",g_conf.remote_port1[0]<<8|g_conf.remote_port1[1]);
	rt_kprintf("remote_port2 %d\r\n",g_conf.remote_port2[0]<<8|g_conf.remote_port2[1]);
	rt_kprintf("remote_port3 %d\r\n",g_conf.remote_port3[0]<<8|g_conf.remote_port3[1]);
	rt_kprintf("protol socket0 %s socket1 %s socket2 %s socket3 %s\r\n",(g_conf.protol[0]==0)?"TCP":"UDP",(g_conf.protol[1]==0)?"TCP":"UDP",(g_conf.protol[2]==0)?"TCP":"UDP",(g_conf.protol[3]==0)?"TCP":"UDP");
	rt_kprintf("mode socket0 %s socket1 %s socket2 %s socket3 %s\r\n",(g_conf.server_mode[0]==0)?"SERVER":"CLIENT",(g_conf.server_mode[1]==0)?"SERVER":"CLIENT",(g_conf.server_mode[2]==0)?"SERVER":"CLIENT",(g_conf.server_mode[3]==0)?"SERVER":"CLIENT");
	rt_kprintf("baud %d.%d.%d.%d\r\n",baud(g_conf.common_baud[0]),baud(g_conf.common_baud[1]),baud(g_conf.common_baud[2]),baud(g_conf.common_baud[3]));
}
int common_w_socket(int dev)
{	
	int len;
	rt_uint8_t common_buf[512],*ptr;
	ptr=common_buf;
	len=rt_device_read(dev, 0, ptr, 512);
	if(len>0)
		rt_data_queue_push(&g_data_queue[dev*2], ptr, len, RT_WAITING_FOREVER);
	return 0;
}

void common_w(void* parameter)
{
	int dev=((int)parameter)/2;
	static int flag=0;
	DBG("common_w %d Enter\r\n",i);
	while (1)
	{
		/* wait receive */
		if (rt_sem_take(&(rx_sem[i]), RT_WAITING_FOREVER) != RT_EOK) continue;
		//DBG("to read in_low %d\r\n",ind_low(dev));
		if(ind[i])
		{	
			if(flag==1)
			{
				print_config();
				flag=0;
			}
			/*socket data transfer,use dma*/
			common_w_socket(dev);
		}
		else
		{
			flag=1;
			/*config data parser*/
			common_rw_config(dev);
		}
	}
}
static rt_err_t common_rx_ind(rt_device_t dev, rt_size_t size)
{
    /* release semaphore to let finsh thread rx data */
	DBG("common_rx_ind %d\r\n",size);
    rt_sem_release(&(rx_sem[which_common_dev(common_dev,dev)]));
    return RT_EOK;
}
static void common_r(void* parameter)
{
    rt_size_t data_size;
    const void *last_data_ptr;
	int dev=(int)parameter;
	while(1)
	{
		rt_data_queue_pop(&g_data_queue[dev], &last_data_ptr, &data_size, RT_WAITING_FOREVER);
		if(data_size!=0 && last_data_ptr)
		{			
			rt_device_write(common_dev[(dev-1)/2], 0, last_data_ptr, data_size);
		}
	}
}

/*init common1,2,3,4 for 4 socket*/
int common_init(int dev)//0 uart , 1 parallel bus, 2 usb
{
	/*init common device*/
	rt_err_t result;
	rt_uint8_t common[10];
	int i;
	/*read config data from internal flash*/

	for(i=3;i<67;i++)
	{
		config_socket0_ip6[i]=0xff;
		config_socket1_ip6[i]=0xff;
		config_socket2_ip6[i]=0xff;
		config_socket3_ip6[i]=0xff;
	}
	config_socket0_ip6[67]=0x26;
	config_socket1_ip6[67]=0x26;
	config_socket2_ip6[67]=0x26;
	config_socket3_ip6[67]=0x26;
	config_socket0_ip6[68]=0xfa;
	config_socket1_ip6[68]=0xfa;
	config_socket2_ip6[68]=0xfa;
	config_socket3_ip6[68]=0xfa;
	for(i=0;i<4;i++)
	{
		//config sem
		rt_sprintf(common,"common_%d_rx",i);
		rt_sem_init(&(rx_sem[i]), common, 0, 0);
		if(dev==DEV_UART)
		{
			if(i==0)
				rt_sprintf(common,"uart0");
			else
				rt_sprintf(common,"uart%d",i+2);
		}
		else if(dev==DEV_BUS)
		{

		}
		else
		{

		}
		DBG("To open ==>%s\n",common);
		//open uart ,parallel ,usb
		common_dev[i] = rt_device_find(common);
		if (common_dev[i] == RT_NULL)
		{
			DBG("app_common: can not find device: %s\n",common);
			return 0;
		}
		if (rt_device_open(common_dev[i], RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) == RT_EOK)
		{
			//create common_w,r thread to read data from socket write to uart,parallel,usb
			//or read from uart,parallel,usb write to socket
			rt_sprintf(common,"common_wx%d",i);
			tid_common_w[i] = rt_thread_create(common,common_w, (void *)(i*2),2048, 20, 10);
			rt_sprintf(common,"common_rx%d",i);
			tid_common_r[i] = rt_thread_create(common,common_r, (void *)(i*2+1),2048, 20, 10);

			rt_device_set_rx_indicate(common_dev[i], common_rx_ind);

			if(tid_common_w[i]!=RT_NULL)
				rt_thread_startup(&tid_common_w[i]);			
			if(tid_common_r[i]!=RT_NULL)
				rt_thread_startup(&tid_common_r[i]);
		}
	}	
	DBG("common_init ok\n");
	return 1;
}
#endif
