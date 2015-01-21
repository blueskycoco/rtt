/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2014, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2014-07-18     ArdaFu       the first version for TM4C129X
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <components.h>

#ifdef RT_USING_LWIP
#include "drv_eth.h"
#endif
#include "led.h"
#include "con_socket.h"
const char buf[]="abcdefghijklmnopqrstuvwxyz\n";
const char buf1[]="123\n";
const char buf2[]="5654d54f\n";

extern void set_if6(char* netif_name, char* ip6_addr);
extern void netio_init(void);

/* led thread entry */
static void led_thread_entry(void* parameter)
{
	//rt_hw_led_init();
	const void *data_ptr;
    rt_size_t data_size;
    const void *last_data_ptr;
	while(1)
	{
		rt_hw_led_on();
		rt_thread_delay(RT_TICK_PER_SECOND);
		rt_hw_led_off();
		rt_thread_delay(RT_TICK_PER_SECOND);
		//rt_data_queue_push(&g_data_queue[0], buf, strlen(buf), 0); 
		//rt_data_queue_push(&g_data_queue[2], buf1, strlen(buf1), 0); 
		//rt_data_queue_push(&g_data_queue[4], buf2, strlen(buf2), 0); 
		//rt_data_queue_push(&g_data_queue[6], buf2, strlen(buf2), RT_WAITING_FOREVER); 
	}
}
static void dump_thread_entry(void* parameter)
{
	const void *data_ptr;
    rt_size_t data_size;
    const void *last_data_ptr;
	int dev=(int)parameter;
	long sent_size=0,receive_size=0;
	  while(1)
	  {
	  		rt_data_queue_push(&g_data_queue[dev-1], buf, strlen(buf), 0); 
			sent_size+=strlen(buf);
			rt_data_queue_pop(&g_data_queue[dev], &last_data_ptr, &data_size, RT_WAITING_FOREVER);
			if(data_size!=0&&last_data_ptr)
			{			
				receive_size+=data_size;
				char *ptr=(char *)rt_malloc((data_size+1)*sizeof(char));
				rt_memcpy(ptr,last_data_ptr,data_size);
				ptr[data_size]='\0';
				rt_kprintf("socket %d sent %d receive %d=>%s\n",dev/2,sent_size,receive_size,ptr);
				rt_free(ptr);
			}
			
	  }
}

/* thread phase init */
void rt_init_thread_entry(void *parameter)
{
    /* Initialization RT-Thread Components */
	int i;
#ifdef RT_USING_LWIP
		rt_hw_tiva_eth_init();
#endif
    rt_components_init();
#ifdef RT_USING_FINSH
    finsh_set_device(RT_CONSOLE_DEVICE_NAME);
#endif
	set_if6("e0","fe80::1");
	netio_init();
	//app_uart_init();
	//uart_init();
	//ring_buffer_init();
	//ping_test("192.168.1.7",5,32);
	g_data_queue=(struct rt_data_queue *)rt_malloc(sizeof(struct rt_data_queue)*8);
	for(i=0;i<6;i++)//0,1 for socket0,2,3 for socket1,4,5 for socket2,6,7 for socket3
		rt_data_queue_init(&g_data_queue[i], 2048, 80, RT_NULL);
	socket_init();
	//ping_test6("fe80::5867:8730:e9e6:d5c5%11",5,32);
	//ping_test6("fe80::483:d903:e2ee:d05e%12",5,32);
	//ping_test("192.168.1.6",5,32);
}

int rt_application_init(void)
{
    rt_thread_t tid;
    rt_thread_t led_thread;
	int i;
	char buf[5];
    tid = rt_thread_create("init",
                           rt_init_thread_entry, RT_NULL,
                           2048, RT_THREAD_PRIORITY_MAX / 3, 20);
    if (tid != RT_NULL) rt_thread_startup(tid);
    /* Create led thread */
    led_thread = rt_thread_create("led",
			    led_thread_entry, RT_NULL,
			    256, 20, 20);
    if(led_thread != RT_NULL)
		  rt_thread_startup(led_thread);
	for(i=0;i<3;i++)
	{
		rt_sprintf(buf,"led%d",i);
		led_thread = rt_thread_create(buf,
					dump_thread_entry, (void *)(i*2+1),
					512, 20, 20);
		if(led_thread != RT_NULL)
			  rt_thread_startup(led_thread);
	}

    return 0;
}
