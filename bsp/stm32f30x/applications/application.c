/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 * 2014-04-27     Bernard      make code cleanup. 
 */

#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>

#include <stdio.h>
#include <stdlib.h>

#ifdef RT_USING_LWIP
#include <lwip/sys.h>
#include <lwip/api.h>
#include <netif/ethernetif.h>
#include "stm32f4xx_eth.h"
#endif
#include "led.h"
#include "ili93xx.h"
#ifdef RT_USING_GDB
#include <gdb_stub.h>
#endif
#ifdef RT_USING_GUIENGINE
#include "rtgui_demo.h"
#include <rtgui/driver.h>
#endif
//#include "cJSON.h"
rt_device_t dev_ch2o;
struct rt_semaphore ch2o_rx_sem;

static rt_uint8_t led_stack[ 1024 ];
static struct rt_thread led_thread;
#if 0
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
		rt_free(old);
	
	return out;
}
#endif
static rt_err_t ch2o_rx_ind(rt_device_t dev, rt_size_t size)
{
	rt_sem_release(&(ch2o_rx_sem));    
	return RT_EOK;
}
void ch2o_rcv(void* parameter)
{	
#define BEGIN 0
#define READ_3 1
#define READ_2 2
#define READ_DATA 3
	int len1=0,m=0,mode=BEGIN;
	int i;
	char buf[256]={0};
	int data_ch2o = 0;
	char ch = 0;
	char *ptr=rt_malloc(32);	
	LCD_Clear(White);		
	while(1)	
	{		
		if (rt_sem_take(&(ch2o_rx_sem), RT_WAITING_FOREVER) != RT_EOK) continue;	
#if 0
		int len=rt_device_read(dev_ch2o, 0, ptr+m, 128);		
		if(len>0)	
		{	
			int i;		
			len1=len1+len;
			if(len1==7)
			{
				rt_kprintf("Get from CH2O:\n");
				for(i=0;i<len1;i++)		
				{		
					rt_kprintf("%x ",ptr[i]);
				}	
				rt_kprintf("\n");
				int data_ch2o=ptr[3]*256+ptr[4];
				memset(buf,0,256);
				rt_sprintf(buf,"AS04-T HCHO: %d.%02d ppm",data_ch2o/100,data_ch2o%100);
				
				for(i=0;i<strlen(buf);i++)
				{
					LCD_PutChar(40+i*8, 160,buf[i],Black,White);
				}
				len1=0;
				m=0;
				//rt_free(ptr);
			}
			else
				m=m+len;
		}		
#else
		int len = rt_device_read(dev_ch2o, 0, &ch, 1);
		//rt_kprintf("ch is %d\n",ch);
		if(len == 1)
		{
			switch (mode)
			{
				case BEGIN:
				{
					if(ch == 0x01)
					{
						mode = READ_3;
						ptr[0] = 0x01;
					}
					else
						mode = BEGIN;
				}
				break;

				case READ_3:
				{
					if(ch == 0x03)
					{
						mode = READ_2;
						ptr[1] = 0x03;
					}
					else
						mode = BEGIN;
				}
				break;

				case READ_2:
				{
					if(ch == 0x02)
					{
						mode = READ_DATA;
						m = 0;
						ptr[2] = 0x02;
					}
					else
						mode = BEGIN;
				}
				break;

				case READ_DATA:
				{
					if(m == 0)
					{
						data_ch2o = ch*256;
						ptr[3] = ch;
						m=1;
					}
					else
					{
						data_ch2o = data_ch2o+ch;
						ptr[4] = ch;
						mode = BEGIN;
						rt_kprintf("Get from CH2O:\n");
						for(i=0;i<5;i++)		
						{		
							rt_kprintf("%d ",ptr[i]);
						}	
						rt_kprintf("\n");
						memset(buf,0,256);
						rt_sprintf(buf,"AS04-T HCHO: %d.%02d ppm",data_ch2o/100,data_ch2o%100);
				
						for(i=0;i<strlen(buf);i++)
						{
							LCD_PutChar(40+i*8, 160,buf[i],Black,White);
						}
					}
				}
				break;
				
				default:
				{
					mode = BEGIN;
				}
				break;
			}
		}
#endif
	}	
}

static void led_thread_entry(void* parameter)
{
    unsigned int count=0;
	char buf[256]={0};
	int i;
	unsigned char read_ch2o[8]={0x01,0x03,0x00,0x00,0x00,0x01,0x84,0x0a};
//	char *tmp=RT_NULL;
 	//tmp=add_item(RT_NULL,"12","34");
    rt_hw_led_init();
	dev_ch2o=rt_device_find("uart1");
	if (rt_device_open(dev_ch2o, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) == RT_EOK)			
	{
		struct serial_configure config; 		
		config.baud_rate=9600;
		config.bit_order = BIT_ORDER_LSB;			
		config.data_bits = DATA_BITS_8; 		
		config.parity	 = PARITY_NONE; 		
		config.stop_bits = STOP_BITS_1; 			
		config.invert	 = NRZ_NORMAL;				
		config.bufsz	 = RT_SERIAL_RB_BUFSZ;			
		rt_device_control(dev_ch2o,RT_DEVICE_CTRL_CONFIG,&config);	
		rt_sem_init(&(ch2o_rx_sem), "ch2o_rx", 0, 0);
		rt_device_set_rx_indicate(dev_ch2o, ch2o_rx_ind);
		rt_thread_startup(rt_thread_create("thread_ch2o",ch2o_rcv, 0,1024, 20, 10));
	}
	rt_kprintf("begin to rcv \n");
    while (1)
    {
        /* led1 on */
		memset(buf,0,256);
#ifndef RT_USING_FINSH
        rt_kprintf("led on, count : %d\r\n",count);
#endif
		rt_sprintf(buf,"AS04-T HCHO : %04f",count);
		//tmp=add_item(tmp,"12",buf);
		//rt_kprintf("==>%s",tmp);
        count++;
        rt_hw_led_on(0);
        rt_thread_delay( RT_TICK_PER_SECOND/2 ); /* sleep 0.5 second and switch to other thread */
		//LCD_Clear(Blue);
        rt_hw_led_on(1);
        rt_thread_delay( RT_TICK_PER_SECOND/2 ); /* sleep 0.5 second and switch to other thread */	
		rt_device_write(dev_ch2o, 0, (void *)read_ch2o, 8);
        /* led1 off */
#ifndef RT_USING_FINSH
        rt_kprintf("led off\r\n");
#endif
		//LCD_Clear(Black);
        rt_hw_led_off(0);
        rt_thread_delay( RT_TICK_PER_SECOND/2 );
		
	   	rt_hw_led_off(1);
        rt_thread_delay( RT_TICK_PER_SECOND/2 );
		//LCD_Clear(Red);
		rt_device_write(dev_ch2o, 0, (void *)read_ch2o, 8);
		/*
		LCD_PutChar(80+0, 160,'H',Black,White);
		LCD_PutChar(80+8, 160,'C',Black,White);
		LCD_PutChar(80+16,160,'H',Black,White);
		LCD_PutChar(80+24,160,'O',Black,White);
		LCD_PutChar(80+32,160,':',Black,White);
		LCD_PutChar(80+40,160,'0',Black,White);
		LCD_PutChar(80+48,160,'.',Black,White);
		LCD_PutChar(80+56,160,'1',Black,White);
		LCD_PutChar(80+64,160,'2',Black,White);*/
		//LCD_DrawLine(0,0,240,320);
    }
}

void rt_init_thread_entry(void* parameter)
{
    /* GDB STUB */
#ifdef RT_USING_GDB
    gdb_set_device("uart6");
    gdb_start();
#endif

    /* LwIP Initialization */
#ifdef RT_USING_LWIP
    {
        extern void lwip_sys_init(void);

        /* register ethernetif device */
        eth_system_device_init();

        rt_hw_stm32_eth_init();

        /* init lwip system */
        lwip_sys_init();
        rt_kprintf("TCP/IP initialized!\n");
    }
#endif
#ifdef RT_USING_GUIENGINE
	{
		rt_device_t device;

		device = rt_device_find("lcd");
		/* re-set graphic device */
		rtgui_graphic_set_device(device);
        
        rt_gui_demo_init();
	}
#endif
}

int rt_application_init()
{
    rt_thread_t tid;

    tid = rt_thread_create("init",
        rt_init_thread_entry, RT_NULL,
        2048, RT_THREAD_PRIORITY_MAX/3, 20);

    if (tid != RT_NULL)
        rt_thread_startup(tid);
	/* init led thread */
	rt_err_t result = rt_thread_init(&led_thread,
							"led",
							led_thread_entry,
							RT_NULL,
							(rt_uint8_t*)&led_stack[0],
							sizeof(led_stack),
							20,
							5);
	if (result == RT_EOK)
	{
		rt_thread_startup(&led_thread);
	}

    return 0;
}

/*@}*/
