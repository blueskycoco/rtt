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
void uart_tx(int type,char *buf,int len);
void uart_init(int type);

char g_rx_param_buf[256];
int g_rx_param_len=0;

ALIGN(RT_ALIGN_SIZE)
  static rt_uint8_t led_stack[ 2048 ];
  static struct rt_thread led_thread;
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
  ls("/");	
#endif
  //stdbmp_decode("/FIGURE1.BMP");
  rt_kprintf("\r\nshow over\r\n");
  //LCD_ShowString(60,50,200,16,16,"Mini STM32");
  while (1)
  {
	/* led1 on */
#ifndef RT_USING_FINSH
	rt_kprintf("led on, count : %d\r\n",count);
#endif
	//LCD_Clear(0XF81F);
	//LCD_ShowString(120,240,24,24,24,"Led onn");
	//uart2_tx("123456\r\n",rt_strlen("123456\r\n"));
	count++;
	rt_hw_led_on(0);
	rt_hw_led_on(1);
	rt_thread_delay( RT_TICK_PER_SECOND/2 ); /* sleep 0.5 second and switch to other thread */

	/* led1 off */
#ifndef RT_USING_FINSH
	rt_kprintf("led off\r\n");
#endif
	//LCD_Clear(0xF800);
	//LCD_ShowString(120,240,24,24,24,"Led off");
	//uart2_tx("654321\r\n",rt_strlen("654321\r\n"));
	rt_hw_led_off(0);
	rt_hw_led_off(1);
	rt_thread_delay( RT_TICK_PER_SECOND/2 );
  }
}
#define UART_TYPE_LCD 0
#define UART_TYPE_PARAM 1

static rt_err_t uart_param_rx_ind(rt_device_t dev, rt_size_t size)
{
  char ch;
  int i=0;	
  while (rt_device_read(dev, 0, &ch, 1) == 1)
  {
	g_rx_param_buf[i]=ch;
	i++;
  }
  g_rx_param_len=i;
  uart_tx(UART_TYPE_PARAM,g_rx_param_buf,g_rx_param_len);
}


void rt_init_thread_entry(void* parameter)
{

#ifdef RT_USING_COMPONENTS_INIT
  /* initialization RT-Thread Components */
  rt_components_init();
#endif

#ifdef  RT_USING_FINSH
  finsh_set_device(RT_CONSOLE_DEVICE_NAME);
#endif  /* RT_USING_FINSH */

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
  //uart2_init();

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

/*@}*/
