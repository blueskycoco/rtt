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
 * 2009-01-05     Bernard      first implementation
 * 2013-11-15     bright       add RCC initial and print RCC freq function
 */
#include <rthw.h>
#include <rtthread.h>

#include "board.h"
#include "usart.h"
#include "adc.h"
#include "timer.h"

/* RT_USING_COMPONENTS_INIT */
#ifdef  RT_USING_COMPONENTS_INIT
#include <components.h>
#endif
/**
 * @addtogroup STM32
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
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}
void config_ITM()
{
	// The stimulus port from which SWO data is received and displayed.
		unsigned int ITM_PORT_BIT0 = 0;
	
		// Has to be calculated according to the CPU speed and the output baud rate
		unsigned int TargetDiv = 5;
	
		unsigned int StimulusRegs;

#if 1
#define ITM_ENA   (*(volatile unsigned int*)0xE0000E00) // ITM Enable
#define ITM_TPR   (*(volatile unsigned int*)0xE0000E40) // Trace Privilege Register
#define ITM_TCR   (*(volatile unsigned int*)0xE0000E80) // ITM Trace Control Reg.
#define ITM_LSR   (*(volatile unsigned int*)0xE0000FB0) // ITM Lock Status Register
#define DHCSR     (*(volatile unsigned int*)0xE000EDF0) // Debug register
#define DEMCR     (*(volatile unsigned int*)0xE000EDFC) // Debug register
#define TPIU_ACPR (*(volatile unsigned int*)0xE0040010) // Async Clock presacler register
#define TPIU_SPPR (*(volatile unsigned int*)0xE00400F0) // Selected Pin Protocol Register
#define DWT_CTRL  (*(volatile unsigned int*)0xE0001000) // DWT Control Register
#define FFCR      (*(volatile unsigned int*)0xE0040304) // Formatter and flush Control Register
	//
	// STIM word and byte acces
#define ITM_STIM_U32  (*(volatile unsigned int*)0xE0000000)
#define ITM_STIM_U8   (*(volatile char*)0xE0000000)

	DEMCR |= ( 1 << 24 );
	ITM_LSR = 0xC5ACCE55;
	StimulusRegs =ITM_ENA;
	StimulusRegs &= ~( 1 << ITM_PORT_BIT0 );
	ITM_ENA = StimulusRegs; // Disable ITM stimulus port
	ITM_TCR = 0;		 // Disable ITM
	TPIU_SPPR = 0x00000002;     // Select NRZ mode
	TPIU_ACPR = TargetDiv - 1;  // Example: 72/48 = 1,5 MHz
	ITM_TPR = 0x00000000;
	DWT_CTRL = 0x400003FE;
	FFCR = 0x00000100;
	ITM_TCR = 0x1000D; // Enable ITM
	ITM_ENA = StimulusRegs | ( 1 << ITM_PORT_BIT0 ); // Enable ITM stimulus port

#else
	
	//
	// Enable access to SWO registers
	//

	//DEMCR |= ( 1 << 24 );
	CoreDebug->DEMCR  |= ( 1 << 24 );
	//ITM_LSR = 0xC5ACCE55;
	ITM->LAR = 0xC5ACCE55;
	//
	// Initially disable ITM and stimulus port
	// To make sure that nothing is transferred via SWO
	// when changing the SWO prescaler etc.
	//
	StimulusRegs =ITM->TER;// ITM_ENA;
	StimulusRegs &= ~( 1 << ITM_PORT_BIT0 );
	//ITM_ENA = StimulusRegs; // Disable ITM stimulus port
	ITM->TER=  StimulusRegs;
	//ITM_TCR = 0;		 // Disable ITM
	ITM->TCR=0;

	//
	// Initialize SWO (prescaler, etc.)
	//
	//TPIU_SPPR = 0x00000002;     // Select NRZ mode
	TPI->SPPR = 0x00000002;
	//TPIU_ACPR = TargetDiv - 1;  // Example: 72/48 = 1,5 MHz
	TPI->ACPR = TargetDiv - 1;
	//ITM_TPR = 0x00000000;
	ITM->TPR = 0x00000000;
	//DWT_CTRL = 0x400003FE;
	DWT->CTRL = 0x400003FE;
	//FFCR = 0x00000100;
	TPI->FFCR = 0x00000100;
	//
	// Enable ITM and stimulus port
	//
	//ITM_TCR = 0x1000D; // Enable ITM
	ITM->TCR = 0x1000D;
	//ITM_ENA = StimulusRegs | ( 1 << ITM_PORT_BIT0 ); // Enable ITM stimulus port
	ITM->TER = StimulusRegs | ( 1 << ITM_PORT_BIT0 );
#endif
}
/**
* @brief  Inserts a delay time.
* @param  nCount: specifies the delay time length.
* @retval None
*/
void Delay(__IO uint32_t nCount)
{
	/* Decrement nCount value */
	while (nCount != 0)
	{
		nCount--;
	}
}

/**
 * This RCC initial for system.
 * use HSI clock source and pll
 * HSI = 8; sysclk = 8/2 * 12 = 48MHZ
 * sysclk source is pllclk
 * AHB prescaler is 1, HCLK = SYSCKL = SystemCoreClock = 48MHZ
 */
static void RCC_Configuration(void)
{
	RCC_DeInit();
	/* setup HSI */
	RCC_HSICmd(ENABLE);
	/* Configure PLL source is HSI */
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);
	RCC_PLLCmd(ENABLE);
	/* Configure SYSCLK source is PLL */
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	/* Conigure AHB prescaler value is 1 */
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	/* Delay for RCC setup */
	Delay(0x3FFFF);
	/* Update SystemCoreClock value from RCC configure */
	SystemCoreClockUpdate();
}

#ifdef PRINT_RCC_FREQ_INFO
/**
 * print RCC freq information
 *
 * for example:
 *
 * SYSCLK_Frequency is 48000000HZ
 * PCLK_Frequency is 48000000HZ
 * HCLK_Frequency is 48000000HZ
 * CECCLK_Frequency is 32786HZ
 * ADCCLK_Frequency is 14000000HZ
 * USART1CLK_Frequency is 48000000HZ
 * I2C1CLK_Frequency is 8000000HZ
 * SystemCoreClock is 48000000HZ
 *
 */
void print_rcc_freq_info(void)
{
	RCC_ClocksTypeDef RCC_ClockFreq;

	RCC_GetClocksFreq(&RCC_ClockFreq);

	rt_kprintf("\nSYSCLK_Frequency is %dHZ", RCC_ClockFreq.SYSCLK_Frequency);
	rt_kprintf("\nPCLK_Frequency is %dHZ", RCC_ClockFreq.PCLK_Frequency);
	rt_kprintf("\nHCLK_Frequency is %dHZ", RCC_ClockFreq.HCLK_Frequency);

	rt_kprintf("\nCECCLK_Frequency is %dHZ", RCC_ClockFreq.CECCLK_Frequency);
	rt_kprintf("\nADCCLK_Frequency is %dHZ", RCC_ClockFreq.ADCCLK_Frequency);
	rt_kprintf("\nUSART1CLK_Frequency is %dHZ", RCC_ClockFreq.USART1CLK_Frequency);
	rt_kprintf("\nI2C1CLK_Frequency is %dHZ", RCC_ClockFreq.I2C1CLK_Frequency);
	rt_kprintf("\nSystemCoreClock is %dHZ\n", SystemCoreClock);
}
#endif

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
/**
 * This function will initial STM32 board.
 */
void rt_hw_board_init()
{
	/* NVIC Configuration */
	NVIC_Configuration();
	

	/* Configure the SysTick */
	RCC_Configuration();
	SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);
	config_ITM();

	/* Initial usart deriver, and set console device */
	//rt_hw_usart_init();
	rt_hw_adc_init();
    	rt_hw_timer_init();
    	//rt_hw_batt_init();
#ifdef RT_USING_CONSOLE
	rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif
	/* Print RCC freq info */
#ifdef PRINT_RCC_FREQ_INFO
	print_rcc_freq_info();
#endif
	/* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
}
void rt_hw_console_output(const char* string)
{
	while (*string)
	{
		ITM_SendChar(*string++);
	}
}

/*@}*/
