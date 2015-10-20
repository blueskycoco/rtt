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
#include "cJSON.h"

static rt_uint8_t led_stack[ 512 ];
static struct rt_thread led_thread;
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


static void led_thread_entry(void* parameter)
{
    unsigned int count=0;
	char buf[256]={0};
	char *tmp=RT_NULL;
 	tmp=add_item(RT_NULL,"12","34");
    rt_hw_led_init();
    while (1)
    {
        /* led1 on */
#ifndef RT_USING_FINSH
        rt_kprintf("led on, count : %d\r\n",count);
#endif
		rt_sprintf(buf,"led on , count : %d",count);
		tmp=add_item(tmp,"12",buf);
		rt_kprintf("==>%s",tmp);
        count++;
        rt_hw_led_on(0);
        rt_thread_delay( RT_TICK_PER_SECOND/2 ); /* sleep 0.5 second and switch to other thread */
		//LCD_Clear(Blue);
        rt_hw_led_on(1);
        rt_thread_delay( RT_TICK_PER_SECOND/2 ); /* sleep 0.5 second and switch to other thread */		
        /* led1 off */
#ifndef RT_USING_FINSH
        rt_kprintf("led off\r\n");
#endif
		//LCD_Clear(Black);
        rt_hw_led_off(0);
        rt_thread_delay( RT_TICK_PER_SECOND/2 );
		LCD_Clear(White);
	   	rt_hw_led_off(1);
        rt_thread_delay( RT_TICK_PER_SECOND/2 );
		//LCD_Clear(Red);

		LCD_DrawLine(0,0,240,320);
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
