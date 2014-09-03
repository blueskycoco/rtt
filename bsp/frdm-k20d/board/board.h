/*
 * File      : board.h
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

// <<< Use Configuration Wizard in Context Menu >>>
#ifndef __BOARD_H__
#define __BOARD_H__

#include <MK20D7.h>
#include "port.h"
#include "flexbus.h"

/* board configuration */
#define K20D_EXT_SRAM          1
#define K20D_EXT_SRAM_BEGIN    0x80000000 /* the begining address of external SRAM */
#define K20D_EXT_SRAM_END      0x80010000 /* the end address of external SRAM */

// <o> Internal SRAM memory size[Kbytes] <8-64>
// <i>Default: 64
#define K20_SRAM_SIZE         64
#define K20_SRAM_END          (0x1FFF0000 + (K20_SRAM_SIZE * 1024))

//#define RT_USING_UART1
#define RT_USING_UART0
//#define RT_USING_UART3

// <o> Console on USART: <0=> no console <1=>USART 1 <2=>USART 2 <3=> USART 3
// <i>Default: 1
#define K20_CONSOLE_USART        4

void rt_hw_board_init(void);

#if K20_CONSOLE_USART == 0
#define CONSOLE_DEVICE "uart0"
#elif K20_CONSOLE_USART == 1
#define CONSOLE_DEVICE "uart1"
#elif K20_CONSOLE_USART == 2
#define CONSOLE_DEVICE "uart2"
#elif K20_CONSOLE_USART == 3
#define CONSOLE_DEVICE "uart3"
#elif K20_CONSOLE_USART == 4
#define CONSOLE_DEVICE "uart4"
#elif K20_CONSOLE_USART == 5
#define CONSOLE_DEVICE "uart5"
#endif

#define FINSH_DEVICE_NAME   CONSOLE_DEVICE


#endif

// <<< Use Configuration Wizard in Context Menu >>>
