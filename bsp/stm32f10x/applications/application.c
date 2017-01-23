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

#ifdef  RT_USING_COMPONENTS_INIT
#include <components.h>
#endif  /* RT_USING_COMPONENTS_INIT */

#ifdef RT_USING_DFS
/* dfs filesystem:ELM filesystem init */
#include <dfs_elm.h>
/* dfs Filesystem APIs */
#include <dfs_fs.h>
#endif
#ifdef RT_USING_FINSH
#include <finsh.h>
#endif
#ifdef RT_USING_RTGUI
#include <rtgui/rtgui.h>
#include <rtgui/rtgui_server.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/driver.h>
#include <rtgui/calibration.h>
#endif

#include "led.h"
#include "usart.h"
#include "esp8266.h"
#include "s.h"
ALIGN(RT_ALIGN_SIZE)
	static rt_uint8_t led_stack[ 2048 ];
	static struct rt_thread led_thread;
	extern void main_loop();
	extern void dgus_loop();
static void led_thread_entry(void* parameter)
{
	unsigned int count=0;
	int i,j;
	int fd;
	int index, length;
	char ch;

	rt_hw_led_init();
	//dillon LCD_Init();
#ifdef RT_USING_FINSH
	//  ls("/");	
#endif
	//stdbmp_decode("/FIGURE1.BMP");
	rt_kprintf("\r\nshow over\r\n");
	//LCD_ShowString(60,50,200,16,16,"Mini STM32");
	while (1)
	{
		/* led1 on */
		//#ifndef RT_USING_FINSH
		//rt_kprintf("led on, count : %d\r\n",count);
		////#endif
		//LCD_Clear(0XF81F);
		//LCD_ShowString(120,240,24,24,24,"Led onn");
		//uart2_tx("123456\r\n",rt_strlen("123456\r\n"));
		count++;
		rt_hw_led_on(0);
		rt_hw_led_on(1);
		rt_thread_delay( RT_TICK_PER_SECOND/2 ); /* sleep 0.5 second and switch to other thread */
		//write_data(0x0101,count);

		/* led1 off */
		//#ifndef RT_USING_FINSH
		//rt_kprintf("led off\r\n");
		//#endif
		//LCD_Clear(0xF800);
		//LCD_ShowString(120,240,24,24,24,"Led off");
		//uart2_tx("654321\r\n",rt_strlen("654321\r\n"));
		rt_hw_led_off(0);
		rt_hw_led_off(1);
		rt_thread_delay( RT_TICK_PER_SECOND/2 );
	}
}


void rt_init_thread_entry(void* parameter)
{

#ifdef RT_USING_COMPONENTS_INIT
	/* initialization RT-Thread Components */
	rt_components_init();
#endif
	delay_init(72);
#ifdef  RT_USING_FINSH
	finsh_set_device(RT_CONSOLE_DEVICE_NAME);
#endif  /* RT_USING_FINSH */
	init_esp8266();

	//main_loop();
	//	dgus_loop();
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
}

int rt_application_init(void)
{
	rt_thread_t init_thread;

	rt_err_t result;
	//uart_init();

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
void s_test(void)
{
	int one_page_max=0;//one page size
	int one_userzone_max=0;//one user zone size
	int userzone_num=0;//userzone num
	unsigned char chip_info[24] = {0};
	unsigned char chip_at88sc0104c[] ={0x3b,0xb2,0x11,0x00,0x10,0x80,0x00,0x01,0x10,0x10};
	unsigned char chip_at88sc0204c[] ={0x3b,0xb2,0x11,0x00,0x10,0x80,0x00,0x02,0x20,0x20};
	unsigned char chip_at88sc0404c[] ={0x3b,0xb2,0x11,0x00,0x10,0x80,0x00,0x04,0x40,0x40};
	unsigned char chip_at88sc0808c[] ={0x3b,0xb2,0x11,0x00,0x10,0x80,0x00,0x08,0x80,0x60};
	unsigned char chip_at88sc1616c[] ={0x3b,0xb2,0x11,0x00,0x10,0x80,0x00,0x16,0x16,0x80};
	unsigned char chip_at88sc3216c[] ={0x3b,0xb3,0x11,0x00,0x00,0x00,0x00,0x32,0x32,0x10};
	unsigned char chip_at88sc6416c[] ={0x3b,0xb3,0x11,0x00,0x00,0x00,0x00,0x64,0x64,0x40};
	unsigned char chip_at88sc12816c[]={0x3b,0xb3,0x11,0x00,0x00,0x00,0x01,0x28,0x28,0x60};
	unsigned char chip_at88sc25616c[]={0x3b,0xb3,0x11,0x00,0x00,0x00,0x02,0x56,0x58,0x60};
	if(get_config(chip_info))
	{
		if(memcmp(chip_info,chip_at88sc0104c,10)==0)
		{
			one_page_max=16;
			one_userzone_max=32;
			userzone_num=4;
			rt_kprintf("\r\nat88sc0104c found\r\n");
		}
		else
			rt_kprintf("not our chip\r\n");
	}
	else
		rt_kprintf("read config failed\r\n");
}
FINSH_FUNCTION_EXPORT(s_test, read s)
/*@}*/
