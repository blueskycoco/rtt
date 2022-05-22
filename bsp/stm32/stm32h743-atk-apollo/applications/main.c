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

/* defined the LED0 pin: PB1 */
#define LED0_PIN    GET_PIN(B, 1)

#define L601_W_DIS	GET_PIN(C, 5)
#define L601_RST_EN	GET_PIN(B, 0)
#define L601_PWR_EN	GET_PIN(B, 1)
#define L601_POW_EN	GET_PIN(B, 2)
#define L601_DTR	GET_PIN(E, 7)
#define L601_CTL_EN	GET_PIN(E, 8)
int main(void)
{
    /* set LED0 pin mode to output */
    //rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
    while (1)
    {
      //  rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
      //  rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);
	//rt_kprintf("dis %d, rst %d, pwr %d, pow %d, dtr %d, ctl %d\n",
	//			L601_W_DIS, L601_RST_EN, L601_PWR_EN,
	//			L601_POW_EN, L601_DTR, L601_CTL_EN);
    }
}
