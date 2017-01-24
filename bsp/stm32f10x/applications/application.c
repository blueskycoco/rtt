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
#endif

#ifdef RT_USING_DFS
#include <dfs_elm.h>
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
#include "cJSON.h"
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t led_stack[ 2048 ];
static struct rt_thread led_thread;
extern void main_loop();
extern void dgus_loop();
void sburn(void);
static void led_thread_entry(void* parameter)
{
	unsigned int count=0;
	int i,j;
	int fd;
	int index, length;
	char ch;

	rt_hw_led_init();
	
	while (1)
	{
		count++;
		rt_hw_led_on(0);
		rt_hw_led_on(1);
		rt_thread_delay( RT_TICK_PER_SECOND/2 );
		rt_hw_led_off(0);
		rt_hw_led_off(1);
		rt_thread_delay( RT_TICK_PER_SECOND/2 );
	}
}


void rt_init_thread_entry(void* parameter)
{

#ifdef RT_USING_COMPONENTS_INIT
	rt_components_init();
#endif
	init_esp8266();
	sburn();

#if defined(RT_USING_DFS) && defined(RT_USING_DFS_ELMFAT)
	if (dfs_mount("sd0", "/", "elm", 0, 0) == 0)
	{
		rt_kprintf("File System initialized!\n");
	}
	else
		rt_kprintf("File System initialzation failed!\n");
#endif
}

int rt_application_init(void)
{
	rt_thread_t init_thread;
	rt_err_t result;

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
void sinit(void)
{
	int one_page_max=0;
	int one_userzone_max=0;
	int userzone_num=0;
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
char *parse_json(char *text,const char *item_str)
{	
	char *out=RT_NULL;
	cJSON *item_json;	
	item_json=cJSON_Parse(text);	
	if (!item_json)
	{
		rt_kprintf("Error before: [%s]\n",cJSON_GetErrorPtr());
	}
	else	
	{	
		if (item_json)
		{	 		
			cJSON *data;	
			data=cJSON_GetObjectItem(item_json,item_str);
			if(data)		
			{			
				int nLen = rt_strlen(data->valuestring);
				//rt_kprintf("%s ,%d %s\n",item_str,nLen,data->valuestring);			
				out=(char *)rt_malloc(nLen+1);		
				rt_memset(out,'\0',nLen+1);	
				rt_memcpy(out,data->valuestring,nLen);	
			}		
			else		
				rt_kprintf("can not find %s\n",item_str);	
		} 
		else	
			rt_kprintf("get %s failed\n",item_str); 
			cJSON_Delete(item_json);	
	}	
	return out;
}

extern rt_err_t rym_null(char *devname);
rt_uint8_t *key = RT_NULL;
rt_uint32_t key_len = 0;
void sburn(void)
{
	rym_null("uart1");
	if (key == RT_NULL || key_len == 0)
	{
		rt_kprintf("no config file got\r\n");
		return;
	}
	parse_json(key, "ROOT_KEY");
}
#ifdef RT_USING_FINSH
FINSH_FUNCTION_EXPORT(sinit, init security)
#endif
