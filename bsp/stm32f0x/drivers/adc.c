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
#include "adc.h"

static void RCC_Configuration(void)
{
	  /* Enable GPIO clock */
	  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB, ENABLE);
	  /* Enable USART clock */
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
}

static void GPIO_Configuration(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;

	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_Init(GPIOB, &GPIO_InitStructure);
  	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);
}
#if 0
void gpb1_isr(void)
{
	  static int a=0;
	  if(a==0)
	  {
			GPIO_SetBits(GPIOA, GPIO_Pin_6);
			a=1;
			rt_kprintf("set buzzer on\r\n");
	  }
	  else
	  {
			a=0;
			GPIO_ResetBits(GPIOA,GPIO_Pin_6);
			rt_kprintf("set buzzer off\r\n");
	  }
}
void key_config()
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;
	  EXTI_InitTypeDef EXTI_InitStructure;


	  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB|RCC_AHBPeriph_GPIOA, ENABLE);
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
	  GPIO_Init(GPIOB, &GPIO_InitStructure);
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);
	  GPIO_SetBits(GPIOA, GPIO_Pin_6);

	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);    /* Configure the SPI interrupt priority */
	  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_1_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStructure);
	  EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	  EXTI_Init(&EXTI_InitStructure);    /* Clear DM9000A EXTI line pending bit */
	  EXTI_ClearITPendingBit(EXTI_Line1);

}
#endif
void rt_hw_adc_init(void)
{
	  ADC_InitTypeDef     ADC_InitStructure;
	 // key_config();
	  RCC_Configuration();
	  GPIO_Configuration();
	  /* ADCs DeInit */  
	  ADC_DeInit(ADC1);

	  /* Initialize ADC structure */
	  ADC_StructInit(&ADC_InitStructure);

	  /* Configure the ADC1 in continous mode withe a resolutuion equal to
	   * 12 bits  */
	  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; 
	  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
	  ADC_Init(ADC1, &ADC_InitStructure); 

	  /* Convert the ADC1 Channel 11 with 239.5 Cycles as
	   * sampling time */ 
	  //ADC_ChannelConfig(ADC1, ADC_Channel_1 , ADC_SampleTime_239_5Cycles);   
	  //ADC_ChannelConfig(ADC1, ADC_Channel_5 , ADC_SampleTime_239_5Cycles);   
	  /* ADC Calibration */
	  ADC_GetCalibrationFactor(ADC1);

	  /* Enable ADCperipheral[PerIdx] */
	  ADC_Cmd(ADC1, ENABLE);     

	  /* Wait the ADRDY falg */
	  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY)); 

}
/*channel 5 for shidu , channel 9 for battery vol */
uint16_t read_adc(unsigned char channel)
{
	  __IO uint16_t  ADC1ConvertedValue = 0, ADC1ConvertedVoltage = 0;
	  /* ADC1 regular Software Start Conv */ 
	  ADC_ChannelConfig(ADC1, channel,ADC_SampleTime_7_5Cycles);
	  ADC_StartOfConversion(ADC1);
	  /* Test EOC flag */
	  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);

	  /* Get ADC1 converted data */
	  ADC1ConvertedValue =ADC_GetConversionValue(ADC1);
	  ADC_StopOfConversion(ADC1);

	  /* Compute the voltage */
	  ADC1ConvertedVoltage = (ADC1ConvertedValue *3300)/0xFFF;
	  rt_kprintf("Channel %d , Voltage %d ==> %d\r\n",channel , ADC1ConvertedVoltage,ADC1ConvertedValue);
	  return ADC1ConvertedVoltage;
}
