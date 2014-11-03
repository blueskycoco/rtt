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
 *
 */

/**
 * @addtogroup k64
 */
/*@{*/

#include <stdio.h>

#include "MK20D7.h"
#include <board.h>
#include <rtthread.h>

#include "led.h"
#include "ili9341.h"

#ifdef RT_USING_LWIP
#include <lwip/sys.h>
#include <lwip/api.h>
#include <netif/ethernetif.h>
#include "stm32_eth.h"
#endif
#ifdef RT_USING_RTGUI
#include <rtgui/rtgui.h>
#include <rtgui/rtgui_server.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/driver.h>
//#include <rtgui/calibration.h>
#endif
#ifdef RT_USING_DFS
#include <dfs_init.h>
#include <dfs_fs.h>

#ifdef RT_USING_DFS_UFFS
#include <dfs_uffs.h>
#include "psram_mtd.h"
#endif
#endif

void rt_init_thread_entry(void* parameter)
{
#ifdef RT_USING_COMPONENTS_INIT
	    /* initialization RT-Thread Components */
	    rt_components_init();
#endif

    /* LwIP Initialization */
#ifdef RT_USING_LWIP
    {
        extern void lwip_sys_init(void);

        /* register ethernetif device */
        eth_system_device_init();

        rt_hw_stm32_eth_init();
        /* re-init device driver */
        rt_device_init_all();

        /* init lwip system */
        lwip_sys_init();
        rt_kprintf("TCP/IP initialized!\n");
    }
#endif

//FS
#ifdef RT_USING_DFS
	dfs_init();
#ifdef RT_USING_MTD_NAND
	nand_mtd_init();
	dfs_uffs_init();

	if (dfs_mount("psram0", "/", "uffs", 0, 0) == 0)
	{
	rt_kprintf("uffs mount / partion ok\n");
	}
	else
	rt_kprintf("uffs mount / partion failed!\n");
#endif
#endif
	rt_hw_lcd_init();
//GUI
#ifdef RT_USING_RTGUI
    {
        extern void rt_hw_lcd_init();
        //extern void rtgui_touch_hw_init(void);

        rt_device_t lcd;

        /* init lcd */
        rt_hw_lcd_init();

        /* init touch panel */
        //rtgui_touch_hw_init();

        /* find lcd device */
        lcd = rt_device_find("lcd");

        /* set lcd device as rtgui graphic driver */
        rtgui_graphic_set_device(lcd);

#ifndef RT_USING_COMPONENTS_INIT
        /* init rtgui system server */
        rtgui_system_server_init();
#endif

        //calibration_set_restore(cali_setup);
        //calibration_set_after(cali_store);
        //calibration_init();
    }
#endif /* #ifdef RT_USING_RTGUI */

}

float f_var1;
float f_var2;
float f_var3;
float f_var4;

ALIGN(RT_ALIGN_SIZE)
static char thread_led1_stack[1024];
struct rt_thread thread_led1;
static void rt_thread_entry_led1(void* parameter)
{
    int n = 0;
//	int stage=0;
    rt_uint32_t offset=0;
//	int i=2000;
//	int delta = 50;
    rt_hw_led_init();
	dac_dma();
	
	/*SIM->SCGC2 |= SIM_SCGC2_DAC0_MASK;
		DAC0->C0 |= DAC_C0_DACEN_MASK
				   |DAC_C0_DACRFS_MASK
				   |DAC_C0_DACTRGSEL_MASK;
		
		DAC0->DAT[0].DATL = stage &  0xFF;
		DAC0->DAT[0].DATH = stage >> 8;
		DAC0->C0 |= DAC_C0_DACSWTRG_MASK;
*/
    while (1)
    {
        //rt_kprintf("LED\t%d\tis shining\r\n",n);
		/*if(i==2000)
			  delta = 50;
			else if(i==4000)
			  delta = -50;
			
			i += delta;
*/
        rt_hw_led_on(n);
        rt_thread_delay(RT_TICK_PER_SECOND/2);
		/*rt_kprintf("i is %d\r\n",i);
		DAC0->DAT[0].DATL = i &  0xFF;
		DAC0->DAT[0].DATH = i >> 8;
		DAC0->C0 |= DAC_C0_DACSWTRG_MASK;*/
	if(!Mem_Check(offset))
		//rt_kprintf("offset %x test ok\r\n",offset);
	//else
		rt_kprintf("offset %x test failed\r\n",offset);
	offset++;
	if(offset==0x10000)
		offset=0;
        rt_hw_led_off(n);
        rt_thread_delay(RT_TICK_PER_SECOND/2);

        n++;
		//stage=stage+100000;
        if (n == (LED_MAX+1))
            n = 0;
    }
}

int rt_application_init()
{
    rt_thread_t init_thread;

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

    //------- init led1 thread
    rt_thread_init(&thread_led1,
                   "led_demo",
                   rt_thread_entry_led1,
                   RT_NULL,
                   &thread_led1_stack[0],
                   sizeof(thread_led1_stack),11,5);
    rt_thread_startup(&thread_led1);

    return 0;
}

/*@}*/
