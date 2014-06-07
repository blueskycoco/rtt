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
#include "timer.h"

static void RCC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  /* Enable the TIM3 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}
void buzzer_ctl(int flag)
{
uint16_t capture = 0,x=0;
  if(flag==1)
  {
  TIM_Cmd(TIM3, ENABLE);
  	if(x==700000)
		x=0;
	else
		x=x+100000;
  	capture = TIM_GetCapture1(TIM3);
	TIM_SetCompare1(TIM3, x);
	rt_kprintf("on capture is %d\r\n",capture);
	/* TIM Interrupts enable */
	//TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
	/* TIM2 enable counter */
	
	//TIM_CtrlPWMOutputs(TIM3, ENABLE);
  }
  else
  {
	/* TIM Interrupts enable */
	//TIM_ITConfig(TIM3, TIM_IT_CC1, DISABLE);
	/* TIM2 enable counter */
	TIM_Cmd(TIM3, DISABLE);
	//TIM_CtrlPWMOutputs(TIM1, DISABLE);
	rt_kprintf("off\r\n");
  }

}
void rt_hw_timer_init(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef        TIM_OCInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
    
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_1);
RCC_Configuration();
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  /* TIM3 Configuration */
  TIM_DeInit(TIM3);
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_OCStructInit(&TIM_OCInitStructure);  
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 100000;
  TIM_TimeBaseStructure.TIM_Prescaler = 0x0;       
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* Output Compare PWM Mode configuration */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;//TIM_OCMode_Timing;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;       
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 5000;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);
  //TIM_SetAutoreload(TIM3,300000);
  //TIM_ARRPreloadConfig(TIM3,ENABLE);
  	TIM_Cmd(TIM3, ENABLE);
	TIM_CtrlPWMOutputs(TIM3, ENABLE);

}
void TIM3_IRQHandler(void)
{
  uint16_t capture = 0;
  rt_interrupt_enter();
  if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)
  {
	TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);

	
	//read_batt();
	capture = TIM_GetCapture1(TIM3);
	TIM_SetCompare1(TIM3, capture + 1000);
	rt_kprintf("capture is %d\r\n",capture);
  }
  rt_interrupt_leave();
}
INIT_DEVICE_EXPORT(rt_hw_timer_init);

