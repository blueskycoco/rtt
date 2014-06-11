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
#define ENV_FLASH_ADDR 	((uint32_t)0x08003c00)
#define FLASH_PAGE_SIZE 	((uint32_t)0x00000400)
#define BATTERY_ADC_CHANNEL	9
#define HUM_ADC_CHANNEL 		5
typedef struct _sys_env{
	/*runtime create*/
	//unsigned char b_hum_on;
	//unsigned char b_temp_on;	
	//unsigned char b_battery_low;
	//unsigned char hum_check_count;
	/*setting from server*/
	unsigned char b_led_alarm_open;
	unsigned char b_temp_buzzer_open;
	unsigned char b_hum_buzzer_open;
	unsigned char hum_alarm_type;/*1,2,3,4 times*/
	unsigned short device_id;
	unsigned short battery_low_val;
	unsigned short hum_judge_val;
	unsigned short temp_low_val;
	unsigned short temp_high_val;
	unsigned short crc;
}sys_env,*psys_env;
sys_env g_sys_env;
#if 0
unsigned char command_up[8][13]={
				{0xF5,0x8A,0x00,0x01,0x00,0x55,0x55,0x01,0x01,0x26,0xfa,0x00,0x00},/*Hum lost*/
				{0xF5,0x8A,0x00,0x02,0x11,0x55,0x55,0x01,0x10,0x26,0xfa,0x00,0x00},/*Hum alarm 1*/
				{0xF5,0x8A,0x00,0x03,0x11,0x55,0x55,0x02,0x13,0x26,0xfa,0x00,0x00},/*Hum alarm 2*/
				{0xF5,0x8A,0x00,0x04,0x11,0x55,0x55,0x03,0x12,0x26,0xfa,0x00,0x00},/*Hum alarm 3*/
				{0xF5,0x8A,0x00,0x05,0x11,0x55,0x55,0x04,0x15,0x26,0xfa,0x00,0x00},/*Hum alarm 4*/
				{0xF5,0x8A,0x00,0x06,0x00,0x55,0x55,0x05,0x05,0x26,0xfa,0x00,0x00},/*Temp lost*/
				{0xF5,0x8A,0x00,0x07,0x11,0x55,0x55,0x05,0x14,0x26,0xfa,0x00,0x00},/*Temp alarm*/
				{0xF5,0x8A,0x00,0x08,0x11,0x55,0x55,0x05,0x17,0x26,0xfa,0x00,0x00},/*Battery low alarm*/
};
unsigned char command_down[11][13]={
				{0xF5,0x8A,0x00,0x09,0x12,0x55,0x55,0x05,0x17,0x26,0xfa,0x00,0x00},/*close Hum buzzer alarm*/
				{0xF5,0x8A,0x00,0x10,0x12,0x55,0x55,0x06,0x14,0x26,0xfa,0x00,0x00},/*open Hum buzzer alarm*/
				{0xF5,0x8A,0x00,0x11,0x12,0x55,0x55,0x07,0x15,0x26,0xfa,0x00,0x00},/*set Hum alarm 1*/
				{0xF5,0x8A,0x00,0x12,0x12,0x55,0x55,0x08,0x1a,0x26,0xfa,0x00,0x00},/*set Hum alarm 2*/
				{0xF5,0x8A,0x00,0x13,0x12,0x55,0x55,0x09,0x1b,0x26,0xfa,0x00,0x00},/*set Hum alarm 3*/
				{0xF5,0x8A,0x00,0x14,0x12,0x55,0x55,0x10,0x02,0x26,0xfa,0x00,0x00},/*set Hum alarm 4*/
				{0xF5,0x8A,0x00,0x15,0x12,0x55,0x55,0x11,0x03,0x26,0xfa,0x00,0x00},/*close led alarm*/
				{0xF5,0x8A,0x00,0x16,0x12,0x55,0x55,0x12,0x00,0x26,0xfa,0x00,0x00},/*open led alarm*/
				{0xF5,0x8A,0x00,0x17,0x12,0x55,0x55,0x13,0x01,0x26,0xfa,0x00,0x00},/*close Temp buzzer alarm*/
				{0xF5,0x8A,0x00,0x18,0x12,0x55,0x55,0x14,0x06,0x26,0xfa,0x00,0x00},/*open Temp buzzer alarm*/
				{0xF5,0x8A,0x00,0x19,0x12,0x55,0x55,0x15,0x17,0x26,0xfa,0x00,0x00},/*set device id,last 0x00,0x00 is id*/

	};
#endif
extern unsigned short ds18b20_read(void);
void save_env()
{
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
	FLASH_ErasePage(ENV_FLASH_ADDR);
	FLASH_ProgramWord(ENV_FLASH_ADDR,(g_sys_env.b_led_alarm_open<<24|g_sys_env.b_temp_buzzer_open<<16|g_sys_env.b_hum_buzzer_open<<8|g_sys_env.hum_alarm_type));
	FLASH_ProgramWord(ENV_FLASH_ADDR+4,(g_sys_env.device_id<<16|g_sys_env.battery_low_val));
	FLASH_ProgramWord(ENV_FLASH_ADDR+8,(g_sys_env.hum_judge_val<<16|g_sys_env.temp_low_val));
	g_sys_env.crc=g_sys_env.b_led_alarm_open+g_sys_env.b_temp_buzzer_open+g_sys_env.b_hum_buzzer_open+g_sys_env.hum_alarm_type+g_sys_env.device_id+g_sys_env.battery_low_val+g_sys_env.hum_judge_val+g_sys_env.temp_low_val+g_sys_env.temp_high_val;

	FLASH_ProgramWord(ENV_FLASH_ADDR+12,(g_sys_env.temp_high_val<<16|g_sys_env.crc));
	FLASH_Lock();
}
int read_env()
{
	rt_memcpy((void *)(&g_sys_env),(const void *)((__IO uint32_t *)ENV_FLASH_ADDR),sizeof(struct _sys_env));
	if((g_sys_env.b_led_alarm_open+g_sys_env.b_temp_buzzer_open+g_sys_env.b_hum_buzzer_open+g_sys_env.hum_alarm_type+g_sys_env.device_id+g_sys_env.battery_low_val+g_sys_env.hum_judge_val+g_sys_env.temp_low_val+g_sys_env.temp_high_val)!=g_sys_env.crc)
		return 0;
	else
		return 1;
}
static void system_thread_check_entry(void* parameter)
{

	unsigned char buf[13]={0xF5,0x8A,0x00,0xff,0xff,0x55,0x55,0xff,0xff,0xff,0xfa,0xff,0xff};
	unsigned short x=0;
	unsigned char hum_check_count=0;
	/*init g_env */
	if(read_env()!=1)
	{
		/*reset default env*/
		g_sys_env.b_led_alarm_open=1;
		g_sys_env.b_temp_buzzer_open=1;
		g_sys_env.b_hum_buzzer_open=1;
		g_sys_env.hum_alarm_type=4;
		g_sys_env.device_id=0x0000;
		g_sys_env.temp_high_val=35;
		g_sys_env.temp_low_val=5;
		g_sys_env.battery_low_val=2000;
		g_sys_env.hum_judge_val=3000;
	}
	
	while(1)
	{
		/*check battery low*/
		x=read_adc(BATTERY_ADC_CHANNEL);
		if(x<g_sys_env.battery_low_val)
		{
			/*send low battery alarm*/
			buf[3]=0x08;			
			buf[4]=0x11;
			buf[7]=0x05;
			buf[8]=0x17;
			buf[9]=(~(buf[1]+buf[2]+buf[3]+buf[4]+buf[5]+buf[6]+buf[7]+buf[8])+1)%128;
			buf[11]=(g_sys_env.device_id&0xff00)>>8;
			buf[12]=g_sys_env.device_id&0xff;
			wifi_send(buf);			
		}
		/*check temp */
		x=ds18b20_read();
		if(x==0)
		{
			buf[3]=0x08;			
			buf[4]=0x11;
			buf[7]=0x05;
			buf[8]=0x17;
			buf[9]=(~(buf[1]+buf[2]+buf[3]+buf[4]+buf[5]+buf[6]+buf[7]+buf[8])+1)%128;
			buf[11]=(g_sys_env.device_id&0xff00)>>8;
			buf[12]=g_sys_env.device_id&0xff;
			wifi_send(buf);			
		}		
		if(x<TEMP_LOW||x>TEMP_HIGH)
			{

			}
		/*check hum*/
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)!=Bit_RESET)
			g_sys_env.b_hum_on=0;
		else
			g_sys_env.b_hum_on=1;
		if(g_sys_env.b_hum_on)
		{
			x=read_adc(HUM_ADC_CHANNEL);
			if(x<HUM_JUDGE)
			{
				if(g_sys_env.hum_check_count <255)
					g_sys_env.hum_check_count++;
			}
			else
				g_sys_env.hum_check_count=0;
		}
		else
		{
			if(g_sys_env.hum_check_count <255)
				g_sys_env.hum_check_count++;
		}

		#if 0
		/**/
		x=ds18b20_read();
		rt_sprintf(buf,"temp %d.%doC\r\nbattery %d\r\nshidu %d\r\n",x>>8,x&0xff,read_adc(9),read_adc(5));
		wifi_send(buf);
		//buzzer_ctl(1);
		rt_hw_led2_on();
		//rt_hw_led1_on();
		rt_thread_delay(RT_TICK_PER_SECOND);
		//buzzer_ctl(0);
		rt_hw_led2_off();
		rt_thread_delay(RT_TICK_PER_SECOND);
		#endif
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
    system_thread = rt_thread_create("system_check",
    		system_thread_check_entry, RT_NULL,
    		512, 20, 20);
    if(system_thread != RT_NULL)
    	rt_thread_startup(system_thread);
    system_thread = rt_thread_create("param_get",
    		system_thread_param_entry, RT_NULL,
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
