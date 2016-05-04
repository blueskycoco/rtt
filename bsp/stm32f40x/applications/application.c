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

//#include <board.h>
#include <rtthread.h>

#ifdef RT_USING_LWIP
#include <lwip/sys.h>
#include <lwip/api.h>
#include <netif/ethernetif.h>
#include "stm32f4xx_eth.h"
#endif
#ifdef RT_USING_DFS
#include <dfs_init.h>
#include <dfs_fs.h>
#include <dfs_posix.h>
#endif
#ifdef RT_USING_DFS_UFFS
#include <dfs_uffs.h>
#endif
#ifdef RT_USING_DFS_ROMFS
#include <dfs_romfs.h>
#endif
#include "led.h"
#ifdef RT_USING_GDB
#include <gdb_stub.h>
#endif
#include "cap.h"
#include "rtc.h"
static rt_uint8_t led_stack[ 512 ];
static struct rt_thread led_thread;
extern void nand_mtd_init();
static void led_thread_entry(void* parameter)
{
    unsigned int count=0;
 
    rt_hw_led_init();
    while (1)
    {
        /* led1 on */
#ifdef RT_USING_FINSH
        //rt_kprintf("led on, count : %d\r\n",count);
#endif
        count++;
        rt_hw_led_on(0);
        rt_thread_delay( RT_TICK_PER_SECOND/2 ); /* sleep 0.5 second and switch to other thread */		
        /* led1 off */
#ifdef RT_USING_FINSH
        //rt_kprintf("led off\r\n");
#endif
        rt_hw_led_off(0);
        rt_thread_delay( RT_TICK_PER_SECOND/2 );
    }
}

void rt_init_thread_entry(void* parameter)
{
    /* GDB STUB */
#ifdef RT_USING_GDB
    gdb_set_device("uart6");
    gdb_start();
#endif
	rt_hw_rtc_init();

#ifdef RT_USING_DFS
		dfs_init();
#ifdef RT_USING_MTD_NAND
		nand_mtd_init();
#endif 
	#if 1
	dfs_uffs_init();
	if (dfs_mount("nand0", "/", "uffs", 0, 0) == 0)
	{
		rt_kprintf("uffs mount / partion ok\n");
		DIR *dir;
		if(dir = opendir("/history")==RT_NULL)
		{
			rt_kprintf("opendir /history failed!\n");
			if(mkdir("/history",0777)==RT_EOK)
				rt_kprintf("mkdir /history OK!\n");
			else
				rt_kprintf("mkdir /history failed!\n");
		}
		else
		{
			closedir(dir);
			rt_kprintf("opendir /history OK!\n");
		}
		#if 0
		if (dfs_mount("nand0", "/history", "uffs", 0, 0) == 0)
		{
			rt_kprintf("uffs mount /nand0 partion ok\n");
		}
		else
		{
			if(mkdir("/history",0777)==RT_EOK)
			{
				if (dfs_mount("nand0", "/history", "uffs", 0, 0) == 0)
				{
					rt_kprintf("uffs mount on /history ok\n");
				}
				else
					rt_kprintf("uffs mount on /history failed!\n");
			}
			else
				rt_kprintf("mkdir /history failed!\n");
		}
		#endif
	}
	else
		rt_kprintf("uffs mount / partion failed!\n");
	#endif
	
#if 0
#if defined(RT_USING_DFS_DEVFS)
			devfs_init();
			if (dfs_mount(RT_NULL, "/dev", "devfs", 0, 0) == 0)
				rt_kprintf("Device File System initialized!\n");
			else
				rt_kprintf("Device File System initialzation failed!\n");
	
        #ifdef RT_USING_NEWLIB
			/* init libc */
			libc_system_init("uart0");
        #endif
#endif
#endif
#endif
	if(init_cap())
		rt_kprintf("init cap failed\n");
	else
		rt_kprintf("init cap ok\n");
//	list_mem();
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
