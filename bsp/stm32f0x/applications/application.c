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
#include "s1.h"
void cb()
{
	rt_kprintf("\r\nCall back called\r\n");
}

void a(unsigned char zone)
{
	ge p;
	int i;
	memset(&p,0xff,sizeof(ge));
	for(i=0;i<32;i++)
	{
		p.user_zone[i]=i;
	}
	for(i=0;i<3;i++)
	{
		p.pw[i]=i;
	}
	for(i=0;i<8;i++)
	{
		p.g[i]=i;
	}
	p.use_g=zone;
	p.use_pw=zone;
	p.zone_index=zone;
	auth(&p,(callback_t *)cb);
}
void r(unsigned char zone)
{
	ge p;
	int i;
	memset(&p,0xff,sizeof(ge));
	for(i=0;i<3;i++)
	{
		p.pw[i]=i;
	}
	for(i=0;i<8;i++)
	{
		p.g[i]=i;
	}
	p.use_g=zone;
	p.use_pw=zone;
	p.zone_index=zone;
	if(read_userzone(&p))
	{
		for(i=0;i<32;i++)
		{
			if(i%8==0 && i!=0)
				AT88DBG("\n");
			AT88DBG("%4X ",p.user_zone[i]);		
		}
	}
	else
	{
		AT88DBG("read user zone failed "); 	
	}
}

static void rt_init_thread_entry(void* parameter)
{
	rt_thread_t system_thread;
	rt_uint8_t buf[256];
	/* Initialization RT-Thread Components */
#ifdef RT_USING_COMPONENTS_INIT
	rt_components_init();
#endif

	/* Set finsh device */
#ifdef  RT_USING_FINSH
	finsh_set_device(RT_CONSOLE_DEVICE_NAME);
#endif  /* RT_USING_FINSH */
	rt_hw_led1_off();
	//cmx865a_init();


	unsigned int count=1000;
	rt_memset(buf,'\0',256);
	rt_hw_led_init();
	//rt_kprintf("led on, count : %d\r\n",count);
	//rt_sprintf(buf,"%s","- RT -    Thread Operating System");
		
	//	ST7585_Write_String(0,5,"- RT -    ");
	//ST7585_Write_String(0,4,"Thread Operating System");
		Draw_bat(3);
	while (1)
	{
		/* led1 on */
//#ifdef RT_USING_FINSH
		rt_kprintf("led on , count : %d\r\n",count);
//#endif
		
		rt_sprintf(buf,"led on , count : %d",count);
		ST7585_Write_String(0,5,buf);

		//test_cmx865a();
		count++;
		rt_hw_led1_off();
		rt_thread_delay( RT_TICK_PER_SECOND/2 ); /* sleep 0.5 second and switch to other thread */

		/* led1 off */
//#ifdef RT_USING_FINSH
		rt_kprintf("led off\r\n");
//#endif
		rt_sprintf(buf,"led off, count : %d",count);
		ST7585_Write_String(0,5,buf);

		rt_hw_led1_on();
		rt_thread_delay( RT_TICK_PER_SECOND/2 );
	}
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
