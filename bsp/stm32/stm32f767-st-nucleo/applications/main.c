/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 * 2019-1-10      e31207077    add stm32f767-st-nucleo bsp
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <fal.h>
#include <stdbool.h>

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
		rt_kprintf("md5[%d] %02x %02x\n", i, decrypt[i], md5_sum[i]);
		if (decrypt[i] != md5_sum[i])
			return false;
	}
	/* compare ota zone with app zone */
	ptr = (uint8_t *)(APP_ADDRESS);
	MD5Init(&md5);
	MD5Update(&md5, ptr, ota_len);
	MD5Final(&md5, decrypt);
	for (i = 0; i < 16; i++) {
		rt_kprintf("app md5[%d] %02x %02x\n", i, decrypt[i], md5_sum[i]);
		if (decrypt[i] != md5_sum[i])
			break;
	}

	/* cp ota zone to app zone as they are different */
	if (i != 16) {
    	static const struct fal_partition *part_dev = NULL;
		if ((part_dev = fal_partition_find("app")) == NULL) {
			rt_kprintf("can't find app zone");
			return false;
		}
		ptr = (uint8_t *)(OTA_ADDRESS + 20);
		for (i = 0; i < ota_len/262144; i++) {
			ptr += i*262144;
			if (fal_partition_erase(part_dev, i*262144, 256*1024) < 0) {
				rt_kprintf("erase app zone failed %d", i*262144);
				return false;
			}
			if (fal_partition_write(part_dev, i*262144,
						(const uint8_t *)ptr, 256 * 1024) < 0) {
				rt_kprintf("write to app zone failed %d", i*262144);
				return false;
			}
		}

		if (ota_len % 262144) {
			ptr += i*262144;
			if (fal_partition_erase(part_dev, i*262144, 256*1024) < 0) {
				rt_kprintf("erase app zone failed %d", i);
				return false;
			}
			if (fal_partition_write(part_dev, i*262144,
						(const uint8_t *)ptr, ota_len - i*262144) < 0) {
				rt_kprintf("write to app zone failed %d", ota_len - i*262144);
				return false;
			}
		}
		/* recheck app zone */	
		ptr = (uint8_t *)(APP_ADDRESS);
		MD5Init(&md5);
		MD5Update(&md5, ptr, ota_len);
		MD5Final(&md5, decrypt);
		for (i = 0; i < 16; i++) {
			rt_kprintf("recheck app md5[%d] %02x %02x\n", i, decrypt[i], md5_sum[i]);
			if (decrypt[i] != md5_sum[i])
				return false;
		}

		return true;
	} else
		rt_kprintf("no need ota\n");

	return false;
}
#endif

int main(void)
{
    /* set LED1 pin mode to output */
    rt_pin_mode(LED1_PIN, PIN_MODE_OUTPUT);
	fal_init();
#ifndef BSP_WORKING_AS_APP
	if (ota())
		rt_kprintf("ota success\n");
	app_boot();
#else
	rt_kprintf("hello 789\n");
#endif
    while (1)
    {
        rt_pin_write(LED1_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED1_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }
}
