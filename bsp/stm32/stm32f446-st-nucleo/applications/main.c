/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

/* defined the LED1/LED2 pin: PA11, PA12 */
#define LED2_PIN    GET_PIN(A, 11)
#define LED3_PIN    GET_PIN(A, 12)
#define KEY1_PIN    GET_PIN(C,  5)
#define KEY2_PIN    GET_PIN(D,  2)

extern void clear_screen(rt_uint16_t color);
void key1_on(void *args)
{
    rt_kprintf("press key1 PC.5!\n");
    clear_screen(0xF81F);
}

void key2_on(void *args)
{
    rt_kprintf("press key2! PD.2\n");
    clear_screen(0xFFE0);
}

int main(void)
{
    /* set LED2 pin mode to output */
    rt_pin_mode(LED2_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LED3_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(KEY2_PIN, PIN_MODE_INPUT_PULLUP);

    rt_pin_attach_irq(KEY1_PIN, PIN_IRQ_MODE_FALLING, key1_on, RT_NULL);
    rt_pin_irq_enable(KEY1_PIN, PIN_IRQ_ENABLE);
    rt_pin_mode(KEY2_PIN, PIN_MODE_INPUT_PULLUP);
    rt_pin_attach_irq(KEY2_PIN, PIN_IRQ_MODE_FALLING, key2_on, RT_NULL);
    rt_pin_irq_enable(KEY2_PIN, PIN_IRQ_ENABLE);

    while (1)
    {
        rt_pin_write(LED2_PIN, PIN_HIGH);
        rt_pin_write(LED3_PIN, PIN_LOW);
        rt_thread_mdelay(500);
        rt_pin_write(LED2_PIN, PIN_LOW);
        rt_pin_write(LED3_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
    }
}
