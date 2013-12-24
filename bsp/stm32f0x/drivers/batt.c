/*
 * File      : batt.c
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
#include "batt.h"

static void RCC_Configuration(void)
{
    /* Enable GPIO clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    /* Enable I2C clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_I2CCLKConfig(RCC_I2C1CLK_HSI);
}

static void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_1);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

}

void rt_hw_batt_init(void)
{
    I2C_InitTypeDef  I2C_InitStructure;
    RCC_Configuration();
    GPIO_Configuration();
    I2C_InitStructure.I2C_Mode = I2C_Mode_SMBusHost;
    I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
    I2C_InitStructure.I2C_DigitalFilter = 0x00;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_Timing = 0x1045061D;

    I2C_Init(I2C1, &I2C_InitStructure);

    I2C_Cmd(I2C1, ENABLE);
}
rt_bool_t check_timeout(int flag)
{
    uint32_t timeout=100000;
    while((I2C_GetFlagStatus(I2C1,flag)!=RESET)&&timeout)
        timeout--;
    if(timeout)
        return RT_TRUE;
    else
        return RT_FALSE;
}
rt_bool_t i2c_smbus_read_block_data(uint8_t addr, uint8_t command, uint8_t len,uint8_t *blk)
{
    int i;
    /*
       I2C_GenerateSTART(I2C1, ENABLE);
       if(!check_timeout(I2C_EVENT_MASTER_MODE_SELECT)) return RT_FALSE;  

       I2C_Send7bitAddress(I2C1, addr<<1, I2C_Direction_Transmitter);
       if(!check_timeout(I2C_EVENT_MASTER_BYTE_TRANSMITTED)) return RT_FALSE;  

       I2C_SendData(I2C1,command);
       if(!check_timeout(I2C_EVENT_MASTER_BYTE_TRANSMITTED)) return RT_FALSE;  

       I2C_GenerateSTART(I2C1, ENABLE);
       if(!check_timeout(I2C_EVENT_MASTER_MODE_SELECT)) return RT_FALSE;  

       I2C_Send7bitAddress(I2C1, addr<<1, I2C_Direction_Receiver);
       if(!check_timeout(I2C_EVENT_MASTER_BYTE_TRANSMITTED)) return RT_FALSE;  
       for(i=0;i<len+1;i++)
       {
       if(!check_timeout(I2C_EVENT_MASTER_BYTE_RECEIVED)) return RT_FALSE;  
       blk[i] = I2C_ReceiveData(I2C1);
       }

       I2C_AcknowledgeConfig(I2C1, DISABLE);

       I2C_GenerateSTOP(I2C1, ENABLE);

       I2C_AcknowledgeConfig(I2C1, ENABLE);
       */
    /* Test on BUSY Flag */
    if(!check_timeout(I2C_ISR_BUSY)) return RT_FALSE;  
    /* Configure slave address, nbytes, reload, end mode and start or
     * stop generation */
    I2C_TransferHandling(I2C1, addr<<1, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);

    if(!check_timeout(I2C_ISR_TXIS)) return RT_FALSE;  
    /* Send Register address */
    I2C_SendData(I2C1, command);

    if(!check_timeout(I2C_ISR_TC)) return RT_FALSE;  
    /* Configure slave address, nbytes, reload, end mode and
     * start or stop generation */
    I2C_TransferHandling(I2C1, addr<<1, len+1, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);

    for(i=0;i<len+1;i++)
    {
        if(!check_timeout(I2C_ISR_RXNE)) return RT_FALSE;  
        /* Read data from RXDR */
        blk[i]= I2C_ReceiveData(I2C1);  
    }

    if(!check_timeout(I2C_ISR_STOPF)) return RT_FALSE;  
    /* Clear STOPF flag */
    I2C_ClearFlag(I2C1, I2C_ICR_STOPCF);
    return RT_TRUE;
}
void read_batt(void)
{
    int32_t data;
    uint8_t byte;
    uint8_t string[15];
    i2c_smbus_read_block_data(0x0B,0x00,2,(uint8_t *)&data);
    rt_kprintf("ManufacturerAccess       %x\r\n",data);
    i2c_smbus_read_block_data(0x0B,0x01,2,(uint8_t *)&data);
    rt_kprintf("RemainingCapacityAlarm   %x\r\n",data);
    i2c_smbus_read_block_data(0x0B,0x02,2,(uint8_t *)&data);
    rt_kprintf("RemainingTimeAlarm       %x\r\n",data);
    i2c_smbus_read_block_data(0x0B,0x03,2,(uint8_t *)&data);
    rt_kprintf("BatteryMode              %x\r\n",data);
    i2c_smbus_read_block_data(0x0B,0x04,2,(uint8_t *)&data);
    rt_kprintf("AtRate                   %x\r\n",data);
    i2c_smbus_read_block_data(0x0B,0x05,2,(uint8_t *)&data);
    rt_kprintf("AtRateTimeToFull         %x\r\n",data);
    i2c_smbus_read_block_data(0x0B,0x06,2,(uint8_t *)&data);
    rt_kprintf("AtRateTimeToEmpty        %x\r\n",data);
    i2c_smbus_read_block_data(0x0B,0x07,2,(uint8_t *)&data);
    rt_kprintf("AtRateOK                 %x\r\n",data);
    i2c_smbus_read_block_data(0x0B,0x08,2,(uint8_t *)&data);
    rt_kprintf("Temperature              %x\r\n",data);
    i2c_smbus_read_block_data(0x0B,0x09,2,(uint8_t *)&data);
    rt_kprintf("Voltage                  %x\r\n",data);
    i2c_smbus_read_block_data(0x0B,0x0a,2,(uint8_t *)&data);
    rt_kprintf("Current                  %x\r\n",data);
    i2c_smbus_read_block_data(0x0B,0x0b,2,(uint8_t *)&data);
    rt_kprintf("AverageCurrent           %x\r\n",data);
    i2c_smbus_read_block_data(0x0B,0x0c,1,(uint8_t *)&byte);
    rt_kprintf("MaxError                 %x\r\n",byte);
    i2c_smbus_read_block_data(0x0B,0x0d,1,(uint8_t *)&byte);
    rt_kprintf("RelativeStateOfCharge    %x\r\n",byte);
    i2c_smbus_read_block_data(0x0B,0x0e,1,(uint8_t *)&byte);
    rt_kprintf("AbsoluteStateOfCharge    %x\r\n",byte);
    i2c_smbus_read_block_data(0x0B,0x0f,2,(uint8_t *)&data);
    rt_kprintf("RemainingCapacity        %x\r\n",data);
    i2c_smbus_read_block_data(0x0B,0x10,2,(uint8_t *)&data);
    rt_kprintf("FullChargeCapacity       %x\r\n",data);
    i2c_smbus_read_block_data(0x0B,0x11,2,(uint8_t *)&data);
    rt_kprintf("RunTimeToEmpty           %x\r\n",data);
    i2c_smbus_read_block_data(0x0B,0x12,2,(uint8_t *)&data);
    rt_kprintf("AverageTimeToEmpty       %x\r\n",data);
    i2c_smbus_read_block_data(0x0B,0x13,2,(uint8_t *)&data);
    rt_kprintf("AverageTimeToFull        %x\r\n",data);
    i2c_smbus_read_block_data(0x0B,0x14,2,(uint8_t *)&data);
    rt_kprintf("ChargingCurrent          %x\r\n",data);
    i2c_smbus_read_block_data(0x0B,0x15,2,(uint8_t *)&data);
    rt_kprintf("ChargingVoltage          %x\r\n",data);
    i2c_smbus_read_block_data(0x0B,0x16,2,(uint8_t *)&data);
    rt_kprintf("BatteryStatus            %x\r\n",data);
    i2c_smbus_read_block_data(0x0B,0x17,2,(uint8_t *)&data);
    rt_kprintf("CycleCount               %x\r\n",data);
    i2c_smbus_read_block_data(0x0B,0x18,2,(uint8_t *)&data);
    rt_kprintf("DesignCapacity           %x\r\n",data);
    i2c_smbus_read_block_data(0x0B,0x19,2,(uint8_t *)&data);
    rt_kprintf("DesignVoltage            %x\r\n",data);
    i2c_smbus_read_block_data(0x0B,0x1a,2,(uint8_t *)&data);
    rt_kprintf("SpecificationInfo        %x\r\n",data);
    i2c_smbus_read_block_data(0x0B,0x1b,2,(uint8_t *)&data);
    rt_kprintf("ManufactureDate          %x\r\n",data);
    i2c_smbus_read_block_data(0x0B,0x1c,2,(uint8_t *)&data);
    rt_kprintf("SerialNumber             %x\r\n",data);
    i2c_smbus_read_block_data(0x0B,0x20,12,(uint8_t *)&string);
    rt_kprintf("ManufacturerName         %s\r\n",string);
    i2c_smbus_read_block_data(0x0B,0x21,8,(uint8_t *)&string);
    rt_kprintf("DeviceName               %s\r\n",string);
    i2c_smbus_read_block_data(0x0B,0x22,5,(uint8_t *)&string);
    rt_kprintf("DeviceChemistry          %s\r\n",string);
    i2c_smbus_read_block_data(0x0B,0x23,15,(uint8_t *)&string);
    rt_kprintf("ManufacturerData         %s\r\n",string);
}
