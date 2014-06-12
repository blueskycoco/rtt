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

#define LOCAL_ALARM_BATTERY_LOW	0
#define LOCAL_ALARM_TEMP_LOST		1
#define LOCAL_ALARM_TEMP			2
#define LOCAL_ALARM_HUM_LOST		3
#define LOCAL_ALARM_HUM_1		4
#define LOCAL_ALARM_HUM_2		5
#define LOCAL_ALARM_HUM_3		6
#define LOCAL_ALARM_HUM_4		7



typedef struct _sys_env{
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
void local_alarm1(void *alarm_type)
{
	switch(*(int *)(alarm_type))
		{
			case LOCAL_ALARM_BATTERY_LOW:
			{	
				buzzer_ctl(1,1000);
				rt_hw_led2_on();
				rt_thread_delay(RT_TICK_PER_SECOND);				
				buzzer_ctl(0);
				rt_hw_led2_off();
			}
			break;
				
			case LOCAL_ALARM_TEMP_LOST:
			{
				buzzer_ctl(1,3000);
				rt_hw_led2_on();
				rt_thread_delay(2*RT_TICK_PER_SECOND);				
				buzzer_ctl(0);
				rt_hw_led2_off();
			}
			break;
			
			case LOCAL_ALARM_TEMP:
			{
				if(g_sys_env.b_temp_buzzer_open)
					buzzer_ctl(1,5000);
				if(g_sys_env.b_led_alarm_open)
					rt_hw_led2_on();
				if(g_sys_env.b_temp_buzzer_open||g_sys_env.b_led_alarm_open)
					rt_thread_delay(3*RT_TICK_PER_SECOND);			
				if(g_sys_env.b_temp_buzzer_open)
					buzzer_ctl(0);
				if(g_sys_env.b_led_alarm_open)
					rt_hw_led2_off();
			}
			break;
			
			case LOCAL_ALARM_HUM_LOST:
			{
				buzzer_ctl(1,7000);
				rt_hw_led2_on();
				rt_thread_delay(4*RT_TICK_PER_SECOND);				
				buzzer_ctl(0);
				rt_hw_led2_off();
			}
			break;
			
			case LOCAL_ALARM_HUM_1:
			{
				if(g_sys_env.b_hum_buzzer_open)
					buzzer_ctl(1,9000);
				if(g_sys_env.b_led_alarm_open)
					rt_hw_led2_on();
				if(g_sys_env.b_hum_buzzer_open||g_sys_env.b_led_alarm_open)
					rt_thread_delay(5*RT_TICK_PER_SECOND);			
				if(g_sys_env.b_hum_buzzer_open)
					buzzer_ctl(0);
				if(g_sys_env.b_led_alarm_open)
					rt_hw_led2_off();
			}
			break;
			
			case LOCAL_ALARM_HUM_2:
			{
				if(g_sys_env.b_hum_buzzer_open)
					buzzer_ctl(1,11000);
				if(g_sys_env.b_led_alarm_open)
					rt_hw_led2_on();
				if(g_sys_env.b_hum_buzzer_open||g_sys_env.b_led_alarm_open)
					rt_thread_delay(6*RT_TICK_PER_SECOND);				
				if(g_sys_env.b_hum_buzzer_open)
					buzzer_ctl(0);
				if(g_sys_env.b_led_alarm_open)
					rt_hw_led2_off();
			}
			break;
			
			case LOCAL_ALARM_HUM_3:
			{
				if(g_sys_env.b_hum_buzzer_open)
					buzzer_ctl(1,13000);
				if(g_sys_env.b_led_alarm_open)
					rt_hw_led2_on();
				if(g_sys_env.b_hum_buzzer_open||g_sys_env.b_led_alarm_open)
					rt_thread_delay(2*RT_TICK_PER_SECOND);				
				if(g_sys_env.b_hum_buzzer_open)
					buzzer_ctl(0);
				if(g_sys_env.b_led_alarm_open)
					rt_hw_led2_off();
			}
			break;
			
			case LOCAL_ALARM_HUM_4:
			{
				if(g_sys_env.b_hum_buzzer_open)
					buzzer_ctl(1,14000);
				if(g_sys_env.b_led_alarm_open)
					rt_hw_led2_on();
				if(g_sys_env.b_hum_buzzer_open||g_sys_env.b_led_alarm_open)
					rt_thread_delay(7*RT_TICK_PER_SECOND);				
				if(g_sys_env.b_hum_buzzer_open)
					buzzer_ctl(0);
				if(g_sys_env.b_led_alarm_open)
					rt_hw_led2_off();
			}
			break;
			default:
				break;
		}
}
void local_alarm(int type)
{	
	rt_thread_t alarm_thread;
	alarm_thread = rt_thread_create("alarm",
		  local_alarm1, (void *)&type,
		  512, 20, 20);
	if(alarm_thread != RT_NULL)
	  rt_thread_startup(alarm_thread);
}
static void system_thread_check_entry(void* parameter)
{

	unsigned char buf[13]={0xF5,0x8A,0x00,0xff,0xff,0x55,0x55,0xff,0xff,0xff,0xfa,0xff,0xff};
	unsigned short x=0;
	unsigned char hum_check_count=0;
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
#endif
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
			local_alarm(LOCAL_ALARM_BATTERY_LOW);
		}
		/*check temp */
		x=ds18b20_read();
		if(x==0)
		{/*Temp interface is disconnect*/
			buf[3]=0x06;			
			buf[4]=0x00;
			buf[7]=0x05;
			buf[8]=0x05;
			buf[9]=(~(buf[1]+buf[2]+buf[3]+buf[4]+buf[5]+buf[6]+buf[7]+buf[8])+1)%128;
			buf[11]=(g_sys_env.device_id&0xff00)>>8;
			buf[12]=g_sys_env.device_id&0xff;
			wifi_send(buf);		
			local_alarm(LOCAL_ALARM_TEMP_LOST);
		}		
		if(x<g_sys_env.temp_low_val||x>g_sys_env.temp_high_val)
		{/*Temp alarm*/
			buf[3]=0x07;			
			buf[4]=0x11;
			buf[7]=0x05;
			buf[8]=0x14;
			buf[9]=(~(buf[1]+buf[2]+buf[3]+buf[4]+buf[5]+buf[6]+buf[7]+buf[8])+1)%128;
			buf[11]=(g_sys_env.device_id&0xff00)>>8;
			buf[12]=g_sys_env.device_id&0xff;
			wifi_send(buf);	
			local_alarm(LOCAL_ALARM_TEMP);
		}
		/*check hum*/
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)!=Bit_RESET)
		{/*Hum interface disconnect*/
			buf[3]=0x01;			
			buf[4]=0x00;
			buf[7]=0x01;
			buf[8]=0x01;
			buf[9]=(~(buf[1]+buf[2]+buf[3]+buf[4]+buf[5]+buf[6]+buf[7]+buf[8])+1)%128;
			buf[11]=(g_sys_env.device_id&0xff00)>>8;
			buf[12]=g_sys_env.device_id&0xff;
			wifi_send(buf);	
			local_alarm(LOCAL_ALARM_HUM_LOST);
		}
		else
		{
			x=read_adc(HUM_ADC_CHANNEL);
			if(x<g_sys_env.hum_judge_val)
			{
				if(hum_check_count <255)
					hum_check_count++;
			}
			else
				hum_check_count=0;
			switch(hum_check_count)
			{
				case 1:
					{/*hum 1 alarm*/
						if(g_sys_env.hum_alarm_type>=1)
						{
						buf[3]=0x02;			
						buf[4]=0x11;
						buf[7]=0x01;
						buf[8]=0x10;
						buf[9]=(~(buf[1]+buf[2]+buf[3]+buf[4]+buf[5]+buf[6]+buf[7]+buf[8])+1)%128;
						buf[11]=(g_sys_env.device_id&0xff00)>>8;
						buf[12]=g_sys_env.device_id&0xff;
						wifi_send(buf);	
						local_alarm(LOCAL_ALARM_HUM_1);
						break;
						}
					}
				case 2:
					{
						if(g_sys_env.hum_alarm_type>=2)
						{
							/*hum 2 alarm*/
							buf[3]=0x03;			
							buf[4]=0x11;
							buf[7]=0x02;
							buf[8]=0x13;
							buf[9]=(~(buf[1]+buf[2]+buf[3]+buf[4]+buf[5]+buf[6]+buf[7]+buf[8])+1)%128;
							buf[11]=(g_sys_env.device_id&0xff00)>>8;
							buf[12]=g_sys_env.device_id&0xff;
							wifi_send(buf);
							local_alarm(LOCAL_ALARM_HUM_2);
							break;

						}
					}
				
				case 3:
					{
						if(g_sys_env.hum_alarm_type>=3)
						{
							/*hum 3 alarm*/
							buf[3]=0x04;			
							buf[4]=0x11;
							buf[7]=0x03;
							buf[8]=0x12;
							buf[9]=(~(buf[1]+buf[2]+buf[3]+buf[4]+buf[5]+buf[6]+buf[7]+buf[8])+1)%128;
							buf[11]=(g_sys_env.device_id&0xff00)>>8;
							buf[12]=g_sys_env.device_id&0xff;
							wifi_send(buf);	
							local_alarm(LOCAL_ALARM_HUM_3);
							break;

						}
					}
				case 4:
					{
						if(g_sys_env.hum_alarm_type>=4)
						{
							/*hum 4 alarm*/
							buf[3]=0x05;			
							buf[4]=0x11;
							buf[7]=0x04;
							buf[8]=0x15;
							buf[9]=(~(buf[1]+buf[2]+buf[3]+buf[4]+buf[5]+buf[6]+buf[7]+buf[8])+1)%128;
							buf[11]=(g_sys_env.device_id&0xff00)>>8;
							buf[12]=g_sys_env.device_id&0xff;
							wifi_send(buf);	
							local_alarm(LOCAL_ALARM_HUM_4);
							break;

						}
					}
				default:
					break;
				
			}
		}
		rt_thread_delay(10*RT_TICK_PER_SECOND);
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
static void system_thread_param_entry(void* parameter)
{
#if 0
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
	//unsigned char buf[13]={0xF5,0x8A,0x00,0xff,0x12,0x55,0x55,0xff,0xff,0xff,0xfa,0xff,0xff};
	unsigned char buf2[13]={0,0,0,0,0,0,0,0,0,0,0,0,0};
	int len=0,x,crc;
	char y;
	while(1)
	{		
		x=wifi_rcv(&y,1);
		if(x!=0)
		{
			buf2[len]=y;			
			len++;
		}
		if(len==13)
		{	/*got command*/
			if((buf2[0]==0xf5)&&(buf2[1]==0x8a)&&(buf2[2]==0x00)&&(buf2[4]==0x12)&&(buf2[5]==0x55)&&(buf2[6]==0x55)&&(buf2[10]==0xfa))
			{
				crc=(~(buf2[1]+buf2[2]+buf2[3]+buf2[4]+buf2[5]+buf2[6]+buf2[7]+buf2[8])+1)%128;
				if((buf2[3]==0x19)&&(buf2[7]==0x15)&&(buf2[8]==0x17)&&(buf2[9]==crc))
				{	/*set device id,last 0x00,0x00 is id*/
					g_sys_env.device_id=(buf2[11]<<8)|buf2[12];
					save_env();
				}
				else
				{
					if((buf2[3]==0x09)&&(buf2[7]==0x05)&&(buf2[8]==0x17)&&(buf2[9]==crc)&&(buf2[11]==(g_sys_env.device_id&0xff00>>8))&&(buf2[12]==(g_sys_env.device_id&0xff)))
					{/*close Hum buzzer alarm*/
						g_sys_env.b_hum_buzzer_open=0;
						save_env();
					}
					if((buf2[3]==0x10)&&(buf2[7]==0x06)&&(buf2[8]==0x14)&&(buf2[9]==crc)&&(buf2[11]==(g_sys_env.device_id&0xff00>>8))&&(buf2[12]==(g_sys_env.device_id&0xff)))
					{/*open Hum buzzer alarm*/
						g_sys_env.b_hum_buzzer_open=1;
						save_env();
					}
					if((buf2[3]==0x11)&&(buf2[7]==0x07)&&(buf2[8]==0x15)&&(buf2[9]==crc)&&(buf2[11]==(g_sys_env.device_id&0xff00>>8))&&(buf2[12]==(g_sys_env.device_id&0xff)))
					{/*set Hum alarm 1*/
						g_sys_env.hum_alarm_type=1;
						save_env();
					}
					if((buf2[3]==0x12)&&(buf2[7]==0x08)&&(buf2[8]==0x1a)&&(buf2[9]==crc)&&(buf2[11]==(g_sys_env.device_id&0xff00>>8))&&(buf2[12]==(g_sys_env.device_id&0xff)))
					{/*set Hum alarm 2*/
						g_sys_env.hum_alarm_type=2;
						save_env();
					}
					if((buf2[3]==0x13)&&(buf2[7]==0x09)&&(buf2[8]==0x1b)&&(buf2[9]==crc)&&(buf2[11]==(g_sys_env.device_id&0xff00>>8))&&(buf2[12]==(g_sys_env.device_id&0xff)))
					{/*set Hum alarm 3*/
						g_sys_env.hum_alarm_type=3;
						save_env();
					}
					if((buf2[3]==0x14)&&(buf2[7]==0x10)&&(buf2[8]==0x02)&&(buf2[9]==crc)&&(buf2[11]==(g_sys_env.device_id&0xff00>>8))&&(buf2[12]==(g_sys_env.device_id&0xff)))
					{/*set Hum alarm 4*/
						g_sys_env.hum_alarm_type=4;
						save_env();
					}
					if((buf2[3]==0x15)&&(buf2[7]==0x11)&&(buf2[8]==0x03)&&(buf2[9]==crc)&&(buf2[11]==(g_sys_env.device_id&0xff00>>8))&&(buf2[12]==(g_sys_env.device_id&0xff)))
					{/*close led alarm*/
						g_sys_env.b_led_alarm_open=0;
						save_env();
					}
					if((buf2[3]==0x16)&&(buf2[7]==0x12)&&(buf2[8]==0x00)&&(buf2[9]==crc)&&(buf2[11]==(g_sys_env.device_id&0xff00>>8))&&(buf2[12]==(g_sys_env.device_id&0xff)))
					{/*open led alarm*/
						g_sys_env.b_led_alarm_open=1;
						save_env();
					}
					if((buf2[3]==0x17)&&(buf2[7]==0x13)&&(buf2[8]==0x01)&&(buf2[9]==crc)&&(buf2[11]==(g_sys_env.device_id&0xff00>>8))&&(buf2[12]==(g_sys_env.device_id&0xff)))
					{/*close Temp buzzer alarm*/
						g_sys_env.b_temp_buzzer_open=0;
						save_env();
					}
					if((buf2[3]==0x18)&&(buf2[7]==0x14)&&(buf2[8]==0x06)&&(buf2[9]==crc)&&(buf2[11]==(g_sys_env.device_id&0xff00>>8))&&(buf2[12]==(g_sys_env.device_id&0xff)))
					{/*open Temp buzzer alarm*/
						g_sys_env.b_temp_buzzer_open=1;
						save_env();
					}
					
				}
				len=0;
				
			}
		}
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
    

    /* Create led thread */    
    system_thread = rt_thread_create("param_get",
    		system_thread_param_entry, RT_NULL,
    		512, 20, 20);
    if(system_thread != RT_NULL)
    	rt_thread_startup(system_thread);
    system_thread = rt_thread_create("system_check",
    		system_thread_check_entry, RT_NULL,
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
