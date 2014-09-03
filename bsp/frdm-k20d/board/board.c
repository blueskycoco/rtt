/*
 * File      : board.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009 RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 
 */

#include <rthw.h>
#include <rtthread.h>

#include "board.h"

#include "drv_uart.h"


/**
 * @addtogroup K64
 */

/*@{*/

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{

}

/*******************************************************************************
 * Function Name  : SysTick_Configuration
 * Description    : Configures the SysTick for OS tick.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void  SysTick_Configuration(void)
{
    SystemCoreClockUpdate();              /* Update Core Clock Frequency        */
    SysTick_Config(SystemCoreClock/RT_TICK_PER_SECOND); /* Generate interrupt each 1 ms       */
}

/**
 * This is the timer interrupt service routine.
 *
 */
void SysTick_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    rt_tick_increase();

    /* leave interrupt */
    rt_interrupt_leave();
}
#if K20D_EXT_SRAM
void EXT_SRAM_Configuration(void)
{
	PORT_ClkEn ( PORTA );
	PORT_ClkEn ( PORTB );
	PORT_ClkEn ( PORTC );
	PORT_ClkEn ( PORTD );

	/* Configure the pins needed to FlexBus Function (Alt 5) */
	/* this example uses low drive strength settings	   */

	//address/data
	PORT_BitFn ( PORTB, PIN_9, FN_5);				// fb_ad[20]
	PORT_BitFn ( PORTB,PIN_10, FN_5);				// fb_ad[19]
	PORT_BitFn ( PORTB,PIN_11, FN_5);				// fb_ad[18]
	PORT_BitFn ( PORTB,PIN_16, FN_5);				// fb_ad[17]
	PORT_BitFn ( PORTB,PIN_17, FN_5);				// fb_ad[16]
	PORT_BitFn ( PORTB,PIN_18, FN_5);				// fb_ad[15]
	PORT_BitFn ( PORTC, PIN_0, FN_5);				// fb_ad[14]
	PORT_BitFn ( PORTC, PIN_1, FN_5);				// fb_ad[13]
	PORT_BitFn ( PORTC, PIN_2, FN_5);				// fb_ad[12]
	PORT_BitFn ( PORTC, PIN_4, FN_5);				// fb_ad[11]	PORT_BitFn ( PORTC, PIN_5, FN_5);				// fb_ad[10]
	PORT_BitFn ( PORTC, PIN_6, FN_5);				// fb_ad[9] PORT_BitFn ( PORTC, PIN_7, FN_5);				// fb_ad[8]
	PORT_BitFn ( PORTC, PIN_8, FN_5);				// fb_ad[7] PORT_BitFn ( PORTC, PIN_9, FN_5);				// fb_ad[6]
	PORT_BitFn ( PORTC,PIN_10, FN_5);				// fb_ad[5]
	PORT_BitFn ( PORTD, PIN_2, FN_5);				// fb_ad[4]
	PORT_BitFn ( PORTD, PIN_3, FN_5);				// fb_ad[3]
	PORT_BitFn ( PORTD, PIN_4, FN_5);				// fb_ad[2]
	PORT_BitFn ( PORTD, PIN_5, FN_5);				// fb_ad[1]
	PORT_BitFn ( PORTD, PIN_6, FN_5);				// fb_ad[0]
	//control signals
//	PORT_BitFn ( PORTC,PIN_16, FN_5);				// fb_be[15:8]
//	PORT_BitFn ( PORTC,PIN_17, FN_5);				// fb_be[ 7:0]
	
	PORT_BitFn ( PORTB,PIN_19, FN_5);				// fb_oe_b
	PORT_BitFn ( PORTC,PIN_11, FN_5);				// fb_rw_b
	PORT_BitFn ( PORTD, PIN_0, FN_5);				// fb_ale
//	PORT_BitFn ( PORTC, PIN_3, FN_5);				// fb_clk_out
	
//	PORT_BitFn ( PORTD, PIN_1, FN_5);				// fb_cs0_b
	PORT_BitFn ( PORTC,PIN_18, FN_5);				// fb_cs2_b
	FLEXBUS_ClkEn();
	FLEXBUS_Init(5);

}
#endif
/**
 * This function will initial Tower board.
 */
void rt_hw_board_init()
{
    /* NVIC Configuration */
    NVIC_Configuration();

    /* Configure the SysTick */
    SysTick_Configuration();
#if K20D_EXT_SRAM
	EXT_SRAM_Configuration();
#endif
    rt_hw_uart_init();

#ifdef RT_USING_CONSOLE
    rt_console_set_device(CONSOLE_DEVICE);
#endif
}

/*@}*/
