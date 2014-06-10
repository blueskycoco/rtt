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
 * 2013-11-15     bright       add init thread and components initial
 */

/**
 * @addtogroup STM32
 */
/*@{*/

#include <stdio.h>

#include <board.h>
#include <rtthread.h>
#ifdef  RT_USING_COMPONENTS_INIT
#include <components.h>
#endif  /* RT_USING_COMPONENTS_INIT */

#include "led.h"
extern unsigned short ds18b20_read(void);
#if 0
/* led thread entry */
static void led_thread_entry(void* parameter)
{
	while(1)
	{
		  rt_hw_led1_on();
		  rt_thread_delay(RT_TICK_PER_SECOND);

		  rt_hw_led1_off();
		  rt_thread_delay(RT_TICK_PER_SECOND);
	}
}
static void led1_thread_entry(void* parameter)
{

	while(1)
	{
		rt_kprintf("temp %x\r\n",read_temp());
		rt_hw_led2_on();
		rt_thread_delay(RT_TICK_PER_SECOND);

		rt_hw_led2_off();
		rt_thread_delay(RT_TICK_PER_SECOND/2);
	}
}
#endif
static void system_thread_entry(void* parameter)
{

	//rt_hw_adc_init();
    	//rt_hw_timer_init();
	//init_18b20();
	unsigned char buf[25];
	int x;
	while(1)
	{
		//rt_kprintf("1234\r\n");
		x=ds18b20_read();
		rt_sprintf(buf,"temp %d.%doC\r\nbattery %d\r\nshidu %d\r\n",x>>8,x&0xff,read_adc(9),read_adc(5));
		//rt_kprintf("temp %d.%doC\r\n",x>>8,x&0xff);
		//rt_kprintf("battery %d\r\n",read_adc(9));
		//rt_kprintf("shidu %d\r\n",read_adc(5));
		//rt_kprintf("%s",buf);
		//wifi_send("temp ");
		//wifi_send(x>>8+40);
		//wifi_send(".");
		//wifi_send(x&0xff+40);
		//wifi_send("oC\r\n");
		wifi_send(buf);
		buzzer_ctl(1);
		rt_hw_led2_on();
		//rt_hw_led1_on();
		rt_thread_delay(RT_TICK_PER_SECOND);
		buzzer_ctl(0);
		rt_hw_led2_off();
		rt_thread_delay(RT_TICK_PER_SECOND);

	}
}
static void rt_init_thread_entry(void* parameter)
{
	rt_thread_t system_thread;

/* Initialization RT-Thread Components */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_init();
#endif

/* Set finsh device */
#ifdef  RT_USING_FINSH
    finsh_set_device(RT_CONSOLE_DEVICE_NAME);
#endif  /* RT_USING_FINSH */
//rt_hw_led1_off();
    /* Create led thread */
    system_thread = rt_thread_create("system",
    		system_thread_entry, RT_NULL,
    		512, 20, 20);
    if(system_thread != RT_NULL)
    	rt_thread_startup(system_thread);

}

int rt_application_init()
{
	rt_thread_t init_thread;

#if (RT_THREAD_PRIORITY_MAX == 32)
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   512, 8, 20);
#else
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   512, 80, 20);
#endif
    if(init_thread != RT_NULL)
    	rt_thread_startup(init_thread);

    return 0;
}


/*@}*/
