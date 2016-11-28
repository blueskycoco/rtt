#include <rtthread.h>
#include <rtdevice.h>
#include <string.h>
#include "stm32f10x.h"
#include "pm25.h"
rt_device_t dev_pm25;
struct rt_semaphore pm25_rx_sem;
/*
 * PM25 module crc check
 * data ,input data 
 * len ,input length
 */
unsigned int FucCheckSum(unsigned char *data,unsigned char len)
{
	#if 0
	unsigned char j = 0,tempq=0;
	data += 1;

	for (j=0; j < (len-2); j++)
	{
		tempq += *data;
		data++;
	}
	tempq = (~tempq)+1;
	return tempq;
	#else
	long crc = 0;
	int j = 0;
	for (j = 0; j < len; j++)
	{
		crc += *data;
		data++;
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
	char str_pm25[4] = {0};
	char str_pm10[4] = {0};
	char *ptr = rt_malloc(128);
	char ch = 0;
	int	 pm25_len = 0, m = 0, crc = 0;
	int  data_pm25 = 0, data_pm10 = 0;
	int  times = 0;
	STATE  state = STATE_INITIAL;
	while (1)	
	{	
		if (rt_device_read(dev_pm25, 0, &ch, 1) == 1)
		{
			if (ch == 0x42 && state == STATE_INITIAL)
			{
				state = STATE_BEGIN;
				ptr[0] = 0x42;
			}
			else if (ch == 0x4d && state == STATE_BEGIN)
			{
				state = STATE_LEN;
				pm25_len = -1;
				ptr[1] = 0x4d;
			}
			else if (state == STATE_LEN)
			{
				if (pm25_len == -1)
				{
					pm25_len = ch;
					ptr[2] = ch;
				}
				else
				{
					pm25_len = pm25_len*256 + ch;
					state = STATE_DATA;
					memset(ptr+4, 0, 124);
					m = pm25_len;
					ptr[3] = ch;
				}
			}
			else if (state == STATE_DATA)
			{
				ptr[m - pm25_len + 4] = ch;
				pm25_len--;
				if (pm25_len == 2)
				{
					state = STATE_CRC;
					crc = -1;
				}
			}
			else if (state == STATE_CRC)
			{
				if (crc == -1)
				{
					crc = ch;
					ptr[22] = ch;
				}
				else
				{
					ptr[23] = ch;
					crc = crc*256 + ch;
					state = STATE_INITIAL;
					if (crc == FucCheckSum(ptr, 22))
					{
						if (times == CAP_DIST_TIME)
						{
							data_pm25 = ptr[12]*256 + ptr[13];
							data_pm10 = ptr[14]*256 + ptr[15];
							//rt_kprintf("\npm25 %d<>pm10 %d\n",data_pm25,data_pm10);
							rt_sprintf(str_pm25, "%03d", data_pm25);
							rt_sprintf(str_pm10, "%03d", data_pm10);
							clear();
							draw(str_pm25, str_pm10);
							if(!display())
							{
								ssd1306_init();
								display();
							}
							times = 0;
						}
						else
							times++;
					}
					else
						rt_kprintf("\npm25 crc error, %d , %d\n",crc, FucCheckSum(ptr, 22));
				}
			}
		}
	}	
}
void ask_pm25()
{
	unsigned char cmd[]={0xff,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79};
	GPIO_SetBits(GPIOE, GPIO_Pin_5);
	rt_thread_delay(100);
	GPIO_ResetBits(GPIOE, GPIO_Pin_5);
	rt_kprintf("ask_pm25\n");
	rt_device_write(dev_pm25, 0, (void *)cmd, sizeof(cmd));
	return ;
}
void pm25_init()
{
	dev_pm25 = rt_device_find("uart3");
	if (rt_device_open(dev_pm25, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) == RT_EOK)			
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		struct serial_configure config; 		
		config.baud_rate = 9600;
		config.bit_order = BIT_ORDER_LSB;			
		config.data_bits = DATA_BITS_8; 		
		config.parity	 = PARITY_NONE; 		
		config.stop_bits = STOP_BITS_1; 			
		config.invert	 = NRZ_NORMAL;				
		config.bufsz	 = RT_SERIAL_RB_BUFSZ;			
		rt_device_control(dev_pm25, RT_DEVICE_CTRL_CONFIG, &config);	
		rt_sem_init(&(pm25_rx_sem), "pm25_rx", 0, 0);
		rt_device_set_rx_indicate(dev_pm25, co2_rx_ind);
		rt_thread_startup(rt_thread_create("pm25", thread_pm25, 0, 512, 20, 10));
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

