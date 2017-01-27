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
void sburn(ppe config);
rt_bool_t write_zone(int zone, pe config);
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
		//rt_hw_led_on(1);
		rt_thread_delay( RT_TICK_PER_SECOND );
		rt_hw_led_off(0);
		//rt_hw_led_off(1);
		rt_thread_delay( RT_TICK_PER_SECOND );
	}
}


void rt_init_thread_entry(void* parameter)
{

#ifdef RT_USING_COMPONENTS_INIT
	rt_components_init();
#endif
	pe burn_config;
	//init_esp8266();
	//sinit();
	sburn(&burn_config);
	write_zone(0,burn_config);
	write_zone(1,burn_config);
	write_zone(2,burn_config);
	write_zone(3,burn_config);

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
			4096, 8, 20);
#else
	init_thread = rt_thread_create("init",
			rt_init_thread_entry, RT_NULL,
			2048, 80, 20);
#endif

	if (init_thread != RT_NULL)
		rt_thread_startup(init_thread);

	return 0;
}
void dump_config(ppe config)
{
	int i=0;
	int j=0;
	for(i=0;i<4;i++)
	{
		rt_kprintf("\nAR[%d]\t",i);
		for(j=0;j<2;j++)
			rt_kprintf("%2x ",config->ar[i][j]);
	}
	for(i=0;i<4;i++)
	{
		rt_kprintf("\nCI[%d]\t",i);
		for(j=0;j<7;j++)
			rt_kprintf("%2x ",config->ci[i][j]);
	}
	for(i=0;i<4;i++)
	{
		rt_kprintf("\nG[%d]\t",i);
		for(j=0;j<8;j++)
			rt_kprintf("%2x ",config->g[i][j]);
	}
	for(i=0;i<8;i++)
	{
		rt_kprintf("\nPW[%d]\t",i);
		for(j=0;j<7;j++)
			rt_kprintf("%2x ",config->pw[i][j]);
	}
	rt_kprintf("\nID\t");
	for(i=0;i<7;i++)
	{
		rt_kprintf("%2x ",config->id[i]);		
	}
	rt_kprintf("\nfuse %2d, num_ar %2d\n",config->fuse,config->num_ar);
}
int one_page_max=16;
int one_userzone_max=32;
int userzone_num=4;
void sinit(void)
{
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
char *parse_id(char *text,const char *item_str)
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
char *parse_obj(char *text,const char *item_str, const char *item_str2)
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
			cJSON *data=RT_NULL;	
			data=cJSON_GetObjectItem(item_json, item_str);
			if (!data)
			{
				rt_kprintf("can not find %s\n", item_str);
				cJSON_Delete(item_json);
				return RT_NULL;
			}
			cJSON *data2=cJSON_GetObjectItem(data,item_str2);
			if(data2)		
			{			
				int nLen = rt_strlen(data2->valuestring);
				//rt_kprintf("%s ,%d %s\n",item_str2,nLen,data2->valuestring);			
				out=(char *)rt_malloc(nLen+1);		
				rt_memset(out,'\0',nLen+1);	
				rt_memcpy(out,data2->valuestring,nLen);					
			}		
			else		
				rt_kprintf("can not find %s\n",item_str2);			
		} 
		else	
			rt_kprintf("get %s failed\n",item_str); 
		cJSON_Delete(item_json);	
	}	
	return out;
}
void str2c(unsigned char *str, unsigned char *c)
{
	char *ptr = str;
	char *p_c = c;
	int i=0;
	//rt_kprintf("strlen str %d\n", strlen(str));
	while (i != strlen(str))
	{
		if (*ptr>='0' && *ptr<='9')
			*p_c = *ptr - '0';
		else if(*ptr >= 'a' && *ptr <= 'f')
			*p_c = *ptr - 'a' + 10;
		else
			*p_c = *ptr - 'A' + 10;
		ptr++;
		i++;
		*p_c = *p_c * 16;
		if (*ptr>='0' && *ptr<='9')
			*p_c += *ptr - '0';
		else if(*ptr >= 'a' && *ptr <= 'f')
			*p_c += *ptr - 'a' + 10;
		else
			*p_c += *ptr - 'A' + 10;
		ptr++;
		//rt_kprintf("p_c %x\n", *p_c);
		p_c++;
		i++;
	}
}
extern rt_err_t rym_null(char *devname);
rt_uint8_t *key = RT_NULL;
rt_uint32_t key_len = 0;
void sburn(ppe config)
{
	int j=0;
	rt_memset(config,0,sizeof(pe));
	unsigned char *str = RT_NULL;
	rym_null("uart1");
	if (key == RT_NULL || key_len == 0)
	{
		rt_kprintf("no config file got\r\n");
		return;
	}
	else
	{
		rt_kprintf("rcv len %d\r\n", key_len);
		for (int i = 0; i < key_len; i++)
		{
			rt_kprintf("%c",key[i]);
			if(key[i]=='\n' ||key[i]=='\r')
				j++;
			else
				j=0;
			if(j==6)
			{
				key[i]='\0';
				break;
			}
		}
	}
	str = parse_id(key, "ID");
	if(str)
	{
		str2c(str, config->id);
		rt_kprintf("ID %s \r\n", str);
		rt_free(str);
	}
	
	str = parse_obj(key, "G", "G0");
	if(str)
	{
		str2c(str, config->g[0]);
		rt_kprintf("G0 %s \r\n", str);
		rt_free(str);
	}
	str = parse_obj(key, "G", "G1");
	if(str)
	{
		str2c(str, config->g[1]);
		rt_kprintf("G1 %s \r\n", str);
		rt_free(str);
	}
	str = parse_obj(key, "G", "G2");
	if(str)
	{
		str2c(str, config->g[2]);
		rt_kprintf("G2 %s \r\n", str);
		rt_free(str);
	}
	str = parse_obj(key, "G", "G3");
	if(str)
	{
		str2c(str, config->g[3]);
		rt_kprintf("G3 %s \r\n", str);
		rt_free(str);
	}
	str = parse_obj(key, "CI", "CI0");
	if(str)
	{
		str2c(str, config->ci[0]);
		rt_kprintf("CI0 %s \r\n", str);
		rt_free(str);
	}
	str = parse_obj(key, "CI", "CI1");
	if(str)
	{
		str2c(str, config->ci[1]);
		rt_kprintf("CI1 %s \r\n", str);
		rt_free(str);
	}
	str = parse_obj(key, "CI", "CI2");
	if(str)
	{
		str2c(str, config->ci[2]);
		rt_kprintf("CI2 %s \r\n", str);
		rt_free(str);
	}
	str = parse_obj(key, "CI", "CI3");
	if(str)
	{
		str2c(str, config->ci[3]);
		rt_kprintf("CI3 %s \r\n", str);
		rt_free(str);
	}
	str = parse_obj(key, "PW", "PWW0");
	if(str)
	{
		str2c(str, config->pw[0]);
		rt_kprintf("PWW0 %s  ", str);
		rt_free(str);
	}	
	str = parse_obj(key, "PW", "PWR0");
	if(str)
	{
		config->pw[0][3]=0xff;
		str2c(str, &(config->pw[0][4]));
		rt_kprintf("PWR0 %s \r\n", str);
		rt_free(str);
	}
	str = parse_obj(key, "PW", "PWW1");
	if(str)
	{
		str2c(str, config->pw[1]);
		rt_kprintf("PWW1 %s  ", str);
		rt_free(str);
	}	
	str = parse_obj(key, "PW", "PWR1");
	if(str)
	{
		config->pw[1][3]=0xff;
		str2c(str, &(config->pw[1][4]));
		rt_kprintf("PWR1 %s \r\n", str);
		rt_free(str);
	}
	str = parse_obj(key, "PW", "PWW2");
	if(str)
	{
		str2c(str, config->pw[2]);
		rt_kprintf("PWW2 %s  ", str);
		rt_free(str);
	}	
	str = parse_obj(key, "PW", "PWR2");
	if(str)
	{
		config->pw[2][3]=0xff;
		str2c(str, &(config->pw[2][4]));
		rt_kprintf("PWR2 %s \r\n", str);
		rt_free(str);
	}
	str = parse_obj(key, "PW", "PWW3");
	if(str)
	{
		str2c(str, config->pw[3]);
		rt_kprintf("PWW3 %s  ", str);
		rt_free(str);
	}	
	str = parse_obj(key, "PW", "PWR3");
	if(str)
	{
		config->pw[3][3]=0xff;
		str2c(str, &(config->pw[3][4]));
		rt_kprintf("PWR3 %s \r\n", str);
		rt_free(str);
	}
	str = parse_obj(key, "PW", "PWW4");
	if(str)
	{
		str2c(str, config->pw[4]);
		rt_kprintf("PWW4 %s  ", str);
		rt_free(str);
	}	
	str = parse_obj(key, "PW", "PWR4");
	if(str)
	{
		config->pw[4][3]=0xff;
		str2c(str, &(config->pw[4][4]));
		rt_kprintf("PWR4 %s \r\n", str);
		rt_free(str);
	}
	str = parse_obj(key, "PW", "PWW5");
	if(str)
	{
		str2c(str, config->pw[5]);
		rt_kprintf("PWW5 %s  ", str);
		rt_free(str);
	}	
	str = parse_obj(key, "PW", "PWR5");
	if(str)
	{
		config->pw[5][3]=0xff;
		str2c(str, &(config->pw[5][4]));
		rt_kprintf("PWR5 %s \r\n", str);
		rt_free(str);
	}
	str = parse_obj(key, "PW", "PWW6");
	if(str)
	{
		str2c(str, config->pw[6]);
		rt_kprintf("PWW6 %s  ", str);
		rt_free(str);
	}	
	str = parse_obj(key, "PW", "PWR6");
	if(str)
	{
		config->pw[6][3]=0xff;
		str2c(str, &(config->pw[6][4]));
		rt_kprintf("PWR6 %s\r\n", str);
		rt_free(str);
	}
	str = parse_obj(key, "PW", "PWW7");
	if(str)
	{
		str2c(str, config->pw[7]);
		rt_kprintf("PWW7 %s  ", str);
		rt_free(str);
	}	
	str = parse_obj(key, "PW", "PWR7");
	if(str)
	{
		config->pw[7][3]=0xff;
		str2c(str, &(config->pw[7][4]));
		rt_kprintf("PWR7 %s \r\n", str);
		rt_free(str);
	}
	str = parse_obj(key, "ZONE", "ZONE0");
	if(str)
	{
		str2c(str, config->buf[0]);
		rt_kprintf("ZONE0 %s \r\n", str);
		rt_free(str);
	}	
	str = parse_obj(key, "ZONE", "ZONE1");
	if(str)
	{
		str2c(str, config->buf[1]);
		rt_kprintf("ZONE1 %s \r\n", str);
		rt_free(str);
	}
	str = parse_obj(key, "ZONE", "ZONE2");
	if(str)
	{
		str2c(str, config->buf[2]);
		rt_kprintf("ZONE2 %s \r\n", str);
		rt_free(str);
	}
	str = parse_obj(key, "ZONE", "ZONE3");
	if(str)
	{
		str2c(str, config->buf[3]);
		rt_kprintf("ZONE3 %s \r\n", str);
		rt_free(str);
	}
	rt_free(key);
	config->ar[0][0] = 0x17;config->ar[0][1] = 0x00;
	config->ar[1][0] = 0x17;config->ar[1][1] = (0x01<<6|0x01);
	config->ar[2][0] = 0x17;config->ar[2][1] = (0x02<<6|0x02);
	config->ar[3][0] = 0x17;config->ar[3][1] = (0x03<<6|0x03);
	config->num_ar = 4;
	config->fuse = 0;
	dump_config(config);
	if (burn(*config))
		rt_kprintf("burn is done\r\n");
	else
		rt_kprintf("burn is failed\r\n");
}

BOOL WriteZone(at88* value)
{
	BOOL rc = FALSE;
	int index=0,local_size,local_addr;
	ge param;
	/*transfer addr to userzone 0 byte to 32768 byte*/
	if((value->addr+value->size)>one_userzone_max*userzone_num)
		return FALSE;
	int user_zone_begin=value->addr/one_userzone_max;
	int user_zone_end=(value->addr+value->size)/one_userzone_max;
	memcpy(param.g,value->g,8);
	memcpy(param.pw,value->pw,3);	
	param.page_size=one_page_max;	
	local_size=value->size;
	local_addr=value->addr;
	for(param.zone_index=user_zone_begin;param.zone_index<user_zone_end;param.zone_index++)
	{		
		param.addr=local_addr%one_userzone_max;
		if((param.addr+local_size)<=one_userzone_max)
			param.len=local_size;
		else
			param.len=one_userzone_max-param.addr;
			
		param.use_g=param.zone_index;
		param.use_pw=param.zone_index;
		//memcpy(param.user_zone,value->data+index,param.len);
		param.user_zone=value->data+index;
		if (!userzone_proc(&param,0))
			return FALSE;
		local_size=local_size-param.len;		
		local_addr=local_addr+param.len;
		index=index+param.len;
	}
	return TRUE;
}

BOOL ReadZone(at88* value)
{
	int index=0,local_size,local_addr;
	ge param;
	/*transfer addr to userzone 0 byte to 32768 byte*/
	if((value->addr+value->size)>one_userzone_max*userzone_num)
		return FALSE;
	int user_zone_begin=value->addr/one_userzone_max;
	int user_zone_end=(value->addr+value->size)/one_userzone_max;
	memcpy(param.g,value->g,8);
	memcpy(param.pw,value->pw,3);
	param.page_size=one_page_max;
	local_size=value->size;
	local_addr=value->addr;
	for(param.zone_index=user_zone_begin;param.zone_index<user_zone_end;param.zone_index++)
	{		
		param.addr=local_addr%one_userzone_max;
		if((param.addr+local_size)<=one_userzone_max)
			param.len=local_size;
		else
			param.len=one_userzone_max-param.addr;
			
			param.use_g=param.zone_index;
			param.use_pw=param.zone_index;
		//memcpy(param.user_zone,value->data+index,param.len);
		param.user_zone=value->data+index;
		/*rt_kprintf("user_zone %x\r\ng %x %x %x %x %x %x %x %x\r\npw %x %x %x\r\npage_size %x\r\nzone_index %x\r\n",
			param.user_zone,param.g[0],param.g[1],param.g[2],param.g[3],param.g[4],param.g[5],param.g[6],param.g[7],
			param.pw[0],param.pw[1],param.pw[2],param.page_size,param.zone_index);*/
		if (!userzone_proc(&param,1))
			return FALSE;
		local_size=local_size-param.len;		
		local_addr=local_addr+param.len;
		index=index+param.len;
	}
	return TRUE;
}
rt_bool_t write_zone(int zone, pe config)
{
	unsigned char buf[32]={0};
	at88 at88;	
	at88.data=buf;
	at88.addr=zone*32;
	at88.size=32;
	rt_memcpy(at88.g, config.g[zone], 8);
	rt_memcpy(at88.pw, config.pw[zone], 3);
	rt_memcpy(buf, config.buf[zone], 32);
//	at88.g[0]=0x01;at88.g[1]=0x02;at88.g[2]=0x03;
//	at88.g[3]=0x04;at88.g[4]=0x05;at88.g[5]=0x06;
//	at88.g[6]=0x07;at88.g[7]=0x08;
//	at88.pw[0]=0x01;at88.pw[1]=0x02;
//	at88.pw[2]=0x03;
//	for(int i = 0; i < 8; i++)
//		rt_kprintf("g[%d] = %d\n",i , at88.g[i]);
	
//	for(int i = 0; i < 3; i++)
//		rt_kprintf("pw[%d] = %d\n",i , at88.pw[i]);
	
//	for(int i = 0; i < 32; i++)
//		rt_kprintf("buf[%d] = %d\n",i , at88.data[i]);
//	return RT_TRUE;
	if (WriteZone(&at88))
	{
		rt_memset(buf,0,32);
		if (ReadZone(&at88))
		{
			rt_kprintf("\nZONE%d\n",zone);
			for (int i = 0; i < 32; i++)
			{
				if(i%8==0 && i!=0)
					rt_kprintf("\n");
				rt_kprintf("%4X ",buf[i]);	
			}
			return RT_TRUE;
		}
	}
	else
		rt_kprintf("WriteReg failed\n");
	return RT_FALSE;
}
#ifdef RT_USING_FINSH
FINSH_FUNCTION_EXPORT(sinit, init security)
#endif
