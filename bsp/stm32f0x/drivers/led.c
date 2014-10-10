/*
 * File      : led.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006-2013, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2013-11-15     bright       the first version
 */

#include "led.h"
/* RT_USING_COMPONENTS_INIT */
#ifdef  RT_USING_COMPONENTS_INIT
#include <components.h>
#endif

/*
LED_GREEN: PC8
LED_RED  : PC9
*/

/* Initial led gpio pin  */
int rt_hw_led_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    /* Enable the GPIO_LED Clock */
    RCC_AHBPeriphClockCmd(/*RCC_AHBPeriph_GPIOC|*/RCC_AHBPeriph_GPIOA, ENABLE);

    /* Configure the GPIO_LED pin */
    //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    //GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOA, &GPIO_InitStructure);/*
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOA, &GPIO_InitStructure);*/
    //GPIO_SetBits(GPIOA, GPIO_Pin_7);
    //rt_hw_led2_off();
    //while(1);
    return 0;
}

/* Initial components for device */
INIT_DEVICE_EXPORT(rt_hw_led_init);
