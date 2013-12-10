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
#include <rtdevice.h>
#include "adc.h"

static void RCC_Configuration(void)
{
    /* Enable GPIO clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    /* Enable USART clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

}

static void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void rt_hw_adc_init(void)
{
    ADC_InitTypeDef     ADC_InitStructure;
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
    ADC_ChannelConfig(ADC1, ADC_Channel_11 , ADC_SampleTime_239_5Cycles);   

    /* ADC Calibration */
    ADC_GetCalibrationFactor(ADC1);

    /* Enable ADCperipheral[PerIdx] */
    ADC_Cmd(ADC1, ENABLE);     

    /* Wait the ADRDY falg */
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY)); 

}

#include "finsh.h"
uint16_t adc(void)
{
    __IO uint16_t  ADC1ConvertedValue = 0, ADC1ConvertedVoltage = 0;
    /* ADC1 regular Software Start Conv */ 
    ADC_StartOfConversion(ADC1);
    /* Test EOC flag */
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);

    /* Get ADC1 converted data */
    ADC1ConvertedValue =ADC_GetConversionValue(ADC1);

    /* Compute the voltage */
    ADC1ConvertedVoltage = (ADC1ConvertedValue *3300)/0xFFF;
    rt_kprintf("Voltage %d ==> %d\r\n",ADC1ConvertedVoltage,ADC1ConvertedValue);
    return ADC1ConvertedVoltage;
}
FINSH_FUNCTION_EXPORT(adc, read adc);
