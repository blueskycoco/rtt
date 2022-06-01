/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-03-05     whj4674672   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <fal.h>
#include <stdbool.h>
#include "infra_compat.h"
#include "mqtt_api.h"
#include "ota_api.h"

/* defined the LED1 pin: PB0 */
#define LED1_PIN    GET_PIN(B, 0)
#ifndef BSP_WORKING_AS_APP
#include <stdint.h>
#include "md5.h"

typedef  void (*jump_app)(void);
#define APP_ADDRESS 	0x08040000	//app
#define APP_LEN_ADDR 	0x08020000	//param
#define APP_MD5_ADDR 	0x08100004
#define OTA_ADDRESS		0x08100000
#define SECTOR_SIZE		(128*1024)
jump_app jump;

static void app_boot()
{
	rt_hw_interrupt_disable();
	if (((*(__IO uint32_t*)(APP_ADDRESS + 4)) & 0xFF000000 ) == 0x08000000)
	{
	//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	//	PWR_BackupAccessCmd(ENABLE);
	//	RTC_WriteBackupRegister(RTC_BKP_DR0, 0x00);
	//	PWR_BackupAccessCmd(DISABLE);
		__set_FAULTMASK(1);
		uint32_t JumpAddress = *(__IO uint32_t*) (APP_ADDRESS + 4);
		jump = (jump_app) JumpAddress;
		__set_MSP(*(__IO uint32_t*) APP_ADDRESS);
		jump();
	}
}

bool ota()
{
	/* verify ota zone */
	MD5_CTX md5; 
	uint8_t decrypt[16] = {0};
	uint32_t i;
	uint8_t *ptr = (uint8_t *)(OTA_ADDRESS);
	uint32_t ota_len = (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | (ptr[3]);
	uint8_t *md5_sum = (uint8_t *)(OTA_ADDRESS + 4);
	rt_kprintf("app len: %d\n", ota_len);
	MD5Init(&md5);
	MD5Update(&md5, ptr + 20, ota_len);
	MD5Final(&md5, decrypt);
	for (i = 0; i < 16; i++) {
		//rt_kprintf("md5[%d] %02x %02x\n", i, decrypt[i], md5_sum[i]);
		if (decrypt[i] != md5_sum[i]) {
			//rt_kprintf("ota zone download part fw\n");
			return false;
		}
	}
	/* compare ota zone with app zone */
	ptr = (uint8_t *)(APP_ADDRESS);
	MD5Init(&md5);
	MD5Update(&md5, ptr, ota_len);
	MD5Final(&md5, decrypt);
	for (i = 0; i < 16; i++) {
		//rt_kprintf("app md5[%d] %02x %02x\n", i, decrypt[i], md5_sum[i]);
		if (decrypt[i] != md5_sum[i]) {
			rt_kprintf("need do ota\n");
			break;
		}
	}

	/* cp ota zone to app zone as they are different */
	if (i != 16) {
    	static const struct fal_partition *part_dev = NULL;
		if ((part_dev = fal_partition_find("app")) == NULL) {
			rt_kprintf("can't find app zone");
			return false;
		}
		ptr = (uint8_t *)(OTA_ADDRESS + 20);
		if (fal_partition_erase(part_dev, 0, ota_len) < 0) {
			rt_kprintf("erase app zone failed %d", ota_len);
			return false;
		}
		if (fal_partition_write(part_dev, 0, (const uint8_t *)ptr, ota_len) < 0) {
			rt_kprintf("write to app zone failed %d", ota_len);
			return false;
		}
		/* recheck app zone */	
		ptr = (uint8_t *)(APP_ADDRESS);
		MD5Init(&md5);
		MD5Update(&md5, ptr, ota_len);
		MD5Final(&md5, decrypt);
		for (i = 0; i < 16; i++) {
			//rt_kprintf("recheck app md5[%d] %02x %02x\n", i, decrypt[i], md5_sum[i]);
			if (decrypt[i] != md5_sum[i]) {
				rt_kprintf("recheck app md5 failed\n");
				return false;
			}
		}

		return true;
	} else
		rt_kprintf("no need ota\n");

	return false;
}
#else
void get_cur_ver()
{
    uint8_t ver[128] = {0};
    static const struct fal_partition *param_dev = NULL;
	if ((param_dev = fal_partition_find("param")) == NULL) {
		rt_kprintf("can't find param zone %d\n", __LINE__);
		return;
	}

	fal_partition_read(param_dev, 0, ver, 128);
	if (ver[0] != 'a' || ver[1] != 'p' || ver[2] != 'p')
		strcpy((char *)ver, "app-1.0.1-20220523.0000");

	rt_kprintf("111 cur version: %s\n", ver);
}
#endif

#define L601_W_DIS	GET_PIN(C, 5)
#define L601_RST_EN	GET_PIN(B, 0)
#define L601_PWR_EN	GET_PIN(B, 1)
#define L601_POW_EN	GET_PIN(B, 2)
#define L601_DTR	GET_PIN(E, 7)
#define L601_CTL_EN	GET_PIN(E, 8)
extern int radar_init();
extern void *pclient;
extern int get_ota_len();
extern void mcu_ota();
int main(void)
{
	uint32_t size_file;
    /* set LED1 pin mode to output */
    rt_pin_mode(LED1_PIN, PIN_MODE_OUTPUT);
	fal_init();
#ifndef BSP_WORKING_AS_APP
	if (ota())
		rt_kprintf("ota success\n");
	app_boot();
#else
	get_cur_ver();
	radar_init();
#endif
    while (1)
    {
      //  rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
      //  rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    if (pclient) {
	    IOT_MQTT_Yield(pclient, 200);
	    if (get_ota_len() > 0) {
	    	mcu_ota();
		}
	}
	//rt_kprintf("dis %d, rst %d, pwr %d, pow %d, dtr %d, ctl %d\n",
	//			L601_W_DIS, L601_RST_EN, L601_PWR_EN,
	//			L601_POW_EN, L601_DTR, L601_CTL_EN);
    }
}
