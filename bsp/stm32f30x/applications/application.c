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
#include "app_types.h"
//#include "cJSON.h"
#define BEGIN 0
#define READ_3 1
#define READ_2 2
#define READ_DATA 3
rt_device_t dev_ch2o;
struct rt_semaphore ch2o_rx_sem;

//static rt_uint8_t led_stack[ 1024 ];
//static struct rt_thread led_thread;
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
char buf[256]={0};
void ch2o_rcv(void* parameter)
{	
	int m=0,mode=BEGIN;
	int i;
	int data_ch2o = 0;
	char ch = 0;
	char    str[6] = {0};
	uint16_t  x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	uint16  color[8] = {WHITE,BLACK,BLUE,BLACK,RED,BLACK,GREEN,BLACK};
	rt_kprintf("ch2o rx \n");
	TFT_ClearScreen(Black);		
	GUI_PutString(FONT_64, 20, 30, "23.5", WHITE, BLACK);

	x1 = 10;
	y1 = 10;

	x2 = 20;
	y2 = 24;
	GUI_HLine(x1, y1, x2, WHITE);
	GUI_VLine(x1, y1, y2, WHITE);
	GUI_Line(x1, y1, x2, y2, WHITE);
	x1 = 1;
	while(1)	
	{		
		if (rt_sem_take(&(ch2o_rx_sem), RT_WAITING_FOREVER) != RT_EOK) continue;	
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
						m=1;
					}
					else
					{
						data_ch2o = data_ch2o+ch;
						mode = BEGIN;
						rt_memset(buf,0,256);
						rt_sprintf(buf,"AS04-T HCHO: %d.%02d ppm",data_ch2o/100,data_ch2o%100);
						rt_kprintf("%s\r\n", buf);	
						for(i=0;i<rt_strlen(buf);i++)
						{
							LCD_PutChar(40+i*8, 160,buf[i],Green, Black);
						}
		x1++;
		rt_kprintf("test 1\r\n");
		rt_sprintf(str, "%d", x1);
		rt_kprintf("test 2\r\n");
		GUI_PutString(FONT_64, 100, 200, str, WHITE, BLACK);
		rt_kprintf("test 3\r\n");

		GUI_DispColor(70, 80, 90, 100, color[x1 % 8]);
		rt_kprintf("test 4\r\n");
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
	}	
}

static void led_thread_entry(void* parameter)
{
    unsigned int count=0;
	unsigned char read_ch2o[8]={0x01,0x03,0x00,0x00,0x00,0x01,0x84,0x0a};
	rt_kprintf("begin to rcv \n");
    while (1)
    {
#ifndef RT_USING_FINSH
//        rt_kprintf("led on, count : %d\r\n",count);
#endif
        count++;
        rt_hw_led_on(0);
        rt_thread_delay( RT_TICK_PER_SECOND/2 ); /* sleep 0.5 second and switch to other thread */
        rt_hw_led_on(1);
        rt_thread_delay( RT_TICK_PER_SECOND/2 ); /* sleep 0.5 second and switch to other thread */	
		rt_device_write(dev_ch2o, 0, (void *)read_ch2o, 8);
#ifndef RT_USING_FINSH
//        rt_kprintf("led off\r\n");
#endif
        rt_hw_led_off(0);
        rt_thread_delay( RT_TICK_PER_SECOND/2 );
		
	   	rt_hw_led_off(1);
        rt_thread_delay( RT_TICK_PER_SECOND/2 );
		rt_device_write(dev_ch2o, 0, (void *)read_ch2o, 8);
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
		//rt_device_t device;

		//device = rt_device_find("lcd");
		/* re-set graphic device */
		//rtgui_graphic_set_device(device);
        
        //rt_gui_demo_init();
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
	/*rt_err_t result = rt_thread_init(&led_thread,
							"led",
							led_thread_entry,
							RT_NULL,
							(rt_uint8_t*)&led_stack[0],
							sizeof(led_stack),
							20,
							5);*/
    rt_hw_led_init();
	TFT_Init();
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
		rt_thread_startup(rt_thread_create("thread_ch2o",ch2o_rcv, 0,1024, RT_THREAD_PRIORITY_MAX/3, 20));
	}
	rt_thread_t led_thread = rt_thread_create("led",
			led_thread_entry,
			RT_NULL,2048, RT_THREAD_PRIORITY_MAX/3,20);
	if (led_thread != RT_NULL)
	{
		rt_thread_startup(led_thread);
	}

    return 0;
}

/*@}*/
