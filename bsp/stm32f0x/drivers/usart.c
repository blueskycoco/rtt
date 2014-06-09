/*
 * File      : usart.c
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

#include <stm32f0xx.h>
//#include <rtdevice.h>
#include "usart.h"

/* USART1 */
#define UART1_GPIO_TX			GPIO_Pin_9
#define UART1_GPIO_TX_SOURCE	GPIO_PinSource9
#define UART1_GPIO_RX			GPIO_Pin_10
#define UART1_GPIO_RX_SOURCE	GPIO_PinSource10
#define UART1_GPIO_AF			GPIO_AF_1
#define UART1_GPIO				GPIOA

/* USART2 */
#define UART2_GPIO_TX			GPIO_Pin_2
#define UART2_GPIO_TX_SOURCE	GPIO_PinSource2
#define UART2_GPIO_RX			GPIO_Pin_3
#define UART2_GPIO_RX_SOURCE	GPIO_PinSource3
#define UART2_GPIO_AF			GPIO_AF_1
#define UART2_GPIO				GPIOA

void uart_config()
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	/* Enable GPIO clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	/* Enable USART clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* Enable GPIO clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	/* Enable USART clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* Connect PXx to USARTx_Tx */
	GPIO_PinAFConfig(UART1_GPIO, UART1_GPIO_TX_SOURCE, UART1_GPIO_AF);

	/* Connect PXx to USARTx_Rx */
	GPIO_PinAFConfig(UART1_GPIO, UART1_GPIO_RX_SOURCE, UART1_GPIO_AF);

	/* Configure USART Tx, Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = UART1_GPIO_TX | UART1_GPIO_RX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(UART1_GPIO, &GPIO_InitStructure);

	/* Connect PXx to USARTx_Tx */
	GPIO_PinAFConfig(UART2_GPIO, UART2_GPIO_TX_SOURCE, UART2_GPIO_AF);

	/* Connect PXx to USARTx_Rx */
	GPIO_PinAFConfig(UART2_GPIO, UART2_GPIO_RX_SOURCE, UART2_GPIO_AF);

	/* Configure USART Tx, Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = UART2_GPIO_TX | UART2_GPIO_RX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(UART2_GPIO, &GPIO_InitStructure);
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the USART Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	/* Enable USART */
	/* enable interrupt */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	NVIC_EnableIRQ(USART1_IRQn);

	USART_Cmd(USART1, ENABLE);
	USART_Init(USART2, &USART_InitStructure);
	/* Enable USART */
	/* enable interrupt */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	NVIC_EnableIRQ(USART2_IRQn);

	USART_Cmd(USART2, ENABLE);

	return ;
}

int uart_send(int index,unsigned char byte)
{
	if(index==0)
	{
		while (!(USART1->ISR & USART_FLAG_TXE));
		USART1->TDR = byte;
	}
	else
	{
		while (!(USART2->ISR & USART_FLAG_TXE));
		USART2->TDR = byte;
	}

}
int uart_recv(int index)
{
	int ch;
	ch = -1;
	if(index==0)
	{
		if (USART1->ISR & USART_FLAG_RXNE)
		{
			ch = USART1->RDR & 0xff;
			
		}
	}
	else	
	{
		if (USART2->ISR & USART_FLAG_RXNE)
		{
			ch = USART2->RDR & 0xff;
		}
	}

	return ch;

}
void USART1_IRQHandler(void)
{

	int ch=-1;
	/* enter interrupt */
	//rt_kprintf("Enter usart1 recv  int\r\n");
	rt_interrupt_enter();
	//USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		
		while (1)
		{
			ch = uart_recv(0);
			if (ch == -1)
			break;

			//serial_ringbuffer_putc(serial->int_rx, ch);
			//rt_kprintf("<< %c\r\n",ch);
		}
		/* clear interrupt */
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
	if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
	{
		/* clear interrupt */
		USART_ClearITPendingBit(USART1, USART_IT_TC);
	}
	//USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	/* leave interrupt */
	rt_interrupt_leave();
}
void USART2_IRQHandler(void)
{

	int ch=-1;
	unsigned char buf[20];
	/* enter interrupt */
	//rt_sprintf(buf,"Enter usart2 recv int %x\r\n",USART_GetITStatus(USART2, USART_IT_RXNE));
	//wifi_send("Enter usart2 recv  int ");
	//wifi_send(buf);
	//wifi_send("\r\n");
	rt_interrupt_enter();
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		//wifi_send("2\r\n");
		while (1)
		{
			ch = uart_recv(1);
			//rt_sprintf(buf,"rcv %c\r\n",ch);
			//wifi_send(buf);
			if (ch == -1)
			break;

			//serial_ringbuffer_putc(serial->int_rx, ch);
			//rt_kprintf("<< %c\r\n",ch);
		}
		/* clear interrupt */
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		//wifi_send("3\r\n");
	}
	if (USART_GetITStatus(USART2, USART_IT_TC) != RESET)
	{
		/* clear interrupt */
		USART_ClearITPendingBit(USART2, USART_IT_TC);
	}

	/* leave interrupt */
	rt_interrupt_leave();
}

void wifi_send(const char *s)
{
	while(*s!='\0')
		uart_send(0,*s++);
}
INIT_DEVICE_EXPORT(uart_config);

