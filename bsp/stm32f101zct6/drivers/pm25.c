#include <rtthread.h>
#include <rtdevice.h>
#include "stm32f10x.h"
#include "pm25.h"
rt_device_t dev_pm25;
struct rt_semaphore pm25_rx_sem;
int data_pm25=0,data_pm10;
unsigned int FucCheckSum(unsigned char *i,unsigned char ln)
{
#if 0
	unsigned char j,tempq=0;
	i+=1;

	for(j=0;j<(ln-2);j++)
		{
		tempq+=*i;
		i++;
		}
	tempq=(~tempq)+1;
	return tempq;
	#else
	long crc=0;
	int j;
	for(j=0;j<ln;j++)
	{
		crc+=*i;
		i++;
	}
	return crc;
	#endif
}
static rt_err_t co2_rx_ind(rt_device_t dev, rt_size_t size)
{
	rt_sem_release(&(pm25_rx_sem));	  
	return RT_EOK;
}
void thread_pm25(void* parameter)
{	
	int len1=0,m=0;
	int got=0,j=0;
	char str_pm25[4]={0};
	char str_pm10[4]={0};
	char *ptr=rt_malloc(128);			
	while(1)	
	{		
		if (rt_sem_take(&(pm25_rx_sem), RT_WAITING_FOREVER) != RT_EOK) continue;		
		int len=rt_device_read(dev_pm25, 0, ptr+m, 128);		
		if(len>0)	
		{	
			int i;		
			len1=len1+len;
			got=0;
			for(i=0;i<len1;i++)
				if(ptr[i]==0x42 && ptr[i+1]==0x4d)
				{
					got=1;
					break;
				}
			if(got)
			{				
				
				if((ptr[i+22]*256+ptr[i+23])==FucCheckSum(ptr+i,22))
				{
					rt_kprintf("\nGet from PM25:\n");
					for(j=i;j<i+0x18;j++)		
					{		
						rt_kprintf("%02X ",ptr[j]);
					}						
					data_pm25=ptr[i+12]*256+ptr[i+13];
					data_pm10=ptr[i+14]*256+ptr[i+15];
					rt_kprintf("\n %d<> %d\n",data_pm25,data_pm10);
					rt_sprintf(str_pm25,"%03d",data_pm25);
					rt_sprintf(str_pm10,"%03d",data_pm10);
					//clear();
					//draw(str_pm25,str_pm10);
					//display();
					len1=0;
					m=0;
					memset(ptr,0,128);
				}
				//else
				//	rt_kprintf("\npm25 crc error, %d , %d\n",(ptr[i+22]*256+ptr[i+23]),FucCheckSum(ptr+i,22));
				
			}
			else
				m=m+len;
		}		
	}	
}
void ask_pm25()
{
	unsigned char cmd[]={0xff,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79};
	//rt_device_write(dev_pm25, 0, (void *)cmd, sizeof(cmd));
	return ;
	GPIO_SetBits(GPIOE, GPIO_Pin_5);
	rt_thread_delay(100);
	GPIO_ResetBits(GPIOE, GPIO_Pin_5);
	rt_kprintf("ask_pm25\n");
}
void pm25_init()
{
	dev_pm25=rt_device_find("uart3");
	if (rt_device_open(dev_pm25, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) == RT_EOK)			
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		struct serial_configure config; 		
		config.baud_rate=9600;
		config.bit_order = BIT_ORDER_LSB;			
		config.data_bits = DATA_BITS_8; 		
		config.parity	 = PARITY_NONE; 		
		config.stop_bits = STOP_BITS_1; 			
		config.invert	 = NRZ_NORMAL;				
		config.bufsz	 = RT_SERIAL_RB_BUFSZ;			
		rt_device_control(dev_pm25,RT_DEVICE_CTRL_CONFIG,&config);	
		rt_sem_init(&(pm25_rx_sem), "pm25_rx", 0, 0);
		rt_device_set_rx_indicate(dev_pm25, co2_rx_ind);
		rt_thread_startup(rt_thread_create("thread_co2",thread_pm25, 0,512, 20, 10));
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5;
	 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 	GPIO_Init(GPIOE, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		GPIO_SetBits(GPIOE, GPIO_Pin_5);
		GPIO_SetBits(GPIOC, GPIO_Pin_3);
		rt_thread_delay(20);
		GPIO_ResetBits(GPIOC, GPIO_Pin_3);
		rt_thread_delay(20);
		GPIO_SetBits(GPIOC, GPIO_Pin_3);
	}
}

