/*
 * File      : led.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 *
 */

#include <MK20D7.h>
#include "led.h"

const rt_uint32_t led_mask[] = {1 << 12, 1 << 13, 1 << 24, 1 << 25};

void rt_hw_led_init(void)
{
    SIM->SCGC5 |= (1 << SIM_SCGC5_PORTC_SHIFT);
    SIM->SCGC5 |= (1 << SIM_SCGC5_PORTE_SHIFT);

    PORTC->PCR[12] &= ~PORT_PCR_MUX_MASK;
    PORTC->PCR[12] |= PORT_PCR_MUX(1);   //PTB12 is GPIO pin

    PORTC->PCR[13] &= ~PORT_PCR_MUX_MASK;
    PORTC->PCR[13] |= PORT_PCR_MUX(1);  //PTB13 is GPIO pin

    PORTE->PCR[24] &= ~PORT_PCR_MUX_MASK;
    PORTE->PCR[24] |= PORT_PCR_MUX(1);  //PTE24 is GPIO pin

    PORTE->PCR[25] &= ~PORT_PCR_MUX_MASK;
    PORTE->PCR[25] |= PORT_PCR_MUX(1);  //PTE25 is GPIO pin

    /* Switch LEDs off and enable output*/
	PTC->PDDR |= GPIO_PDDR_PDD(led_mask[1] | led_mask[0]);
	PTE->PDDR |= GPIO_PDDR_PDD(led_mask[3] |led_mask[2]);

    rt_hw_led_off(LED_RED);
    rt_hw_led_off(LED_GREEN);
    rt_hw_led_off(LED_BLUE);
    rt_hw_led_off(LED_MAX);
}

void rt_hw_led_uninit(void)
{
    PORTC->PCR[12] &= ~PORT_PCR_MUX_MASK;

    PORTC->PCR[13] &= ~PORT_PCR_MUX_MASK;

    PORTE->PCR[24] &= ~PORT_PCR_MUX_MASK;

    PORTE->PCR[25] &= ~PORT_PCR_MUX_MASK;
}

void rt_hw_led_on(rt_uint32_t n)
{
    	if(n==LED_BLUE||n==LED_RED)
        		PTC->PCOR |= led_mask[n];
	else
		PTE->PCOR |= led_mask[n];	
}

void rt_hw_led_off(rt_uint32_t n)
{
    	if(n==LED_BLUE||n==LED_RED)
        		PTC->PSOR |= led_mask[n];
	else
		PTE->PSOR |= led_mask[n];
}
