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
 * 2013-07-12     aozima       update for auto initial.
 */

/**
 * @addtogroup STM32
 */
/*@{*/

#include <board.h>
#include <rtthread.h>

#ifdef RT_USING_DFS
/* dfs filesystem:ELM filesystem init */
#include <dfs_elm.h>
/* dfs Filesystem APIs */
#include <dfs_fs.h>
#endif

#ifdef RT_USING_RTGUI
#include <rtgui/rtgui.h>
#include <rtgui/rtgui_server.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/driver.h>
#include <rtgui/calibration.h>
#endif

#include "led.h"
#include "power.h"
static rt_uint8_t lcd_stack[ 512 ];
static struct rt_thread lcd_thread;
static rt_uint8_t cc1101_stack[ 512 ];
static struct rt_thread cc1101_thread;

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t led_stack[ 512 ];
static struct rt_thread led_thread;
static void led_thread_entry(void* parameter)
{
    unsigned int count=0;
 
    rt_hw_led_init();

    while (1)
    {
        /* led1 on */
#ifndef RT_USING_FINSH
        //rt_kprintf("led on, count : %d\r\n",count);
#endif
        count++;
        rt_hw_led_on(0);
        rt_thread_delay( RT_TICK_PER_SECOND/2 ); /* sleep 0.5 second and switch to other thread */
        rt_hw_led_on(1);
        rt_thread_delay( RT_TICK_PER_SECOND/2 ); /* sleep 0.5 second and switch to other thread */
        rt_hw_led_on(2);
        rt_thread_delay( RT_TICK_PER_SECOND/2 ); /* sleep 0.5 second and switch to other thread */
        rt_hw_led_on(3);
        rt_thread_delay( RT_TICK_PER_SECOND/2 ); /* sleep 0.5 second and switch to other thread */

        /* led1 off */
#ifndef RT_USING_FINSH
        //rt_kprintf("led off\r\n");
#endif
        rt_hw_led_off(0);
        rt_thread_delay( RT_TICK_PER_SECOND/2 );
	   rt_hw_led_off(1);
        rt_thread_delay( RT_TICK_PER_SECOND/2 );
	   rt_hw_led_off(2);
        rt_thread_delay( RT_TICK_PER_SECOND/2 );
	   rt_hw_led_off(3);
        rt_thread_delay( RT_TICK_PER_SECOND/2 );
    }
}
static rt_uint8_t power_stack[ 1024 ];
static struct rt_thread power_thread;
static void power_thread_entry(void* parameter)
{
    unsigned int count=0;
//SD_PowerON();
 //return ;
    power_man_init(100,2000);

    while (1)
    {
        	   rt_thread_delay(100);
	   power_man_timer_poll(100,2000);
	   power_man_timer_interrupt();
    }
}
static void cc1101_thread_entry(void* parameter)
{
    unsigned int count=0;
//	rt_kprintf("Enter cc1101 init\r\n");
    cc1101_init();
uint8_t buf[10],buf1[10],i;
//for(i=0;i<10;i++)
//	buf[i]=
sprintf(buf,"%s","123456789a");
//rt_kprintf("Enter cc1101 end\r\n");
    while (1)
    {
    //rt_kprintf("Enter cc1101 send\r\n");
    	for(i=0;i<10;i++)
		{
			if(buf[i]==255)
				buf[i]=0;
			buf[i]=buf[i]+1;
    		}
	//rt_thread_delay(20);
	cc1101_send((uint8_t *)buf,10);
	//cc1101_recv(31);
	//rt_kprintf("Enter cc1101 send 2\r\n");
	//rt_thread_delay(20);
	cc1101_recv(100);
	
    }
}
static void lcd_thread_entry(void* parameter)
{
	int val1=0,val2=1,val3=2,val4=3,val5=4,val6=5;
	uint8_t bat1=20,bat2=0;
	u8 str[100];	/*将数字信息填充到str里*/
	sprintf(str,"%d%d%d%d%d.%d",val1,val2,val3,val4,val5,val6);
	/*初始化ssd1306*/    
	ssd1306_init();	/*绘制缓冲区，包含电池信息和数字信息*/
	draw(bat1,bat2,str);	/*打开显示*/
	display();
	while(1){
		val1++;
		val2++;
		val3++;
		val4++;
		val5++;
		val6++;
		bat1++;
		bat2++;
		sprintf(str,"%d%d%d%d%d.%d",val1,val2,val3,val4,val5,val6);
		draw(bat1,bat2,str);
		display();
		rt_thread_delay(10);
		if(val1==9)
			val1=0;
		if(val2==9)
			val2=0;
		if(val3==9)
			val3=0;
		if(val4==9)
			val4=0;
		if(val5==9)
			val5=0;
		if(val6==9)
			val6=0;
		if(bat1==100)
			bat1=0;
		if(bat2==100)
			bat2=0;
		}
}
#ifdef RT_USING_RTGUI
rt_bool_t cali_setup(void)
{
    rt_kprintf("cali setup entered\n");
    return RT_FALSE;
}

void cali_store(struct calibration_data *data)
{
    rt_kprintf("cali finished (%d, %d), (%d, %d)\n",
               data->min_x,
               data->max_x,
               data->min_y,
               data->max_y);
}
#endif /* RT_USING_RTGUI */

void rt_init_thread_entry(void* parameter)
{
#ifdef RT_USING_COMPONENTS_INIT
    /* initialization RT-Thread Components */
    rt_components_init();
#endif

    /* Filesystem Initialization */
#if defined(RT_USING_DFS) && defined(RT_USING_DFS_ELMFAT)
    /* mount sd card fat partition 1 as root directory */
    if (dfs_mount("sd0", "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("File System initialized!\n");
    }
    else
        rt_kprintf("File System initialzation failed!\n");
#endif  /* RT_USING_DFS */

#ifdef RT_USING_RTGUI
    {
        extern void rt_hw_lcd_init();
        extern void rtgui_touch_hw_init(void);

        rt_device_t lcd;

        /* init lcd */
        rt_hw_lcd_init();

        /* init touch panel */
        rtgui_touch_hw_init();

        /* find lcd device */
        lcd = rt_device_find("lcd");

        /* set lcd device as rtgui graphic driver */
        rtgui_graphic_set_device(lcd);

#ifndef RT_USING_COMPONENTS_INIT
        /* init rtgui system server */
        rtgui_system_server_init();
#endif

        calibration_set_restore(cali_setup);
        calibration_set_after(cali_store);
        calibration_init();
    }
#endif /* #ifdef RT_USING_RTGUI */
}

int rt_application_init(void)
{
    rt_thread_t init_thread;
    
    rt_err_t result;

   
 //   while(1);
    rt_hw_led_init();
#if 0
    /* init led thread */
    result = rt_thread_init(&led_thread,
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
    
/* init led thread */
    result = rt_thread_init(&lcd_thread,
                            "oled",
                            lcd_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&lcd_stack[0],
                            sizeof(lcd_stack),
                            20,
                            5);
    if (result == RT_EOK)
    {
       rt_thread_startup(&lcd_thread);
    }
    result = rt_thread_init(&cc1101_thread,
                            "cc1101",
                            cc1101_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&cc1101_stack[0],
                            sizeof(cc1101_stack),
                            20,
                            5);
    if (result == RT_EOK)
    {
       rt_thread_startup(&cc1101_thread);
    }
    #else
    result = rt_thread_init(&power_thread,
                            "power",
                            power_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&power_stack[0],
                            sizeof(power_stack),
                            20,
                            5);
    if (result == RT_EOK)
    {
        rt_thread_startup(&power_thread);
    }
    #endif
#if (RT_THREAD_PRIORITY_MAX == 32)
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, 8, 20);
#else
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, 80, 20);
#endif

    if (init_thread != RT_NULL)
        rt_thread_startup(init_thread);

    return 0;
}

/*@}*/
