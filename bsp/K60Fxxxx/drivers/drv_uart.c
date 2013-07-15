/*
 * File      : drv_uart.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2013, RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://openlab.rt-thread.com/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2013-07-11     reynolds     port to TWR-K60F120M
 */


#include "drv_uart.h"

static struct rt_serial_device _k60_serial;  //abstracted serial for RTT
static struct serial_ringbuffer _k60_int_rx; //UART send buffer area

struct k60_serial_device
{
    /* UART base address */
    UART_Type *baseAddress;

    /* UART IRQ Number */
    int irq_num;

    /* device config */
    struct serial_configure config;
};

//hardware abstract device
static struct k60_serial_device _k60_node =
{
    (UART_Type *)UART5,
    k60_uasrt_irq_num,
};

static rt_err_t _configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    unsigned int reg_C1 = 0,reg_BDH = 0,reg_BDL = 0,reg_S2;
    unsigned int cal_SBR = 0;
    UART_Type *uart_reg;

    uart_reg = ((struct k60_serial_device *)serial->parent.user_data)->baseAddress;
    cal_SBR = 60000000 / (16 * cfg->baud_rate);
    reg_BDH = (cal_SBR & 0x1FFF) >> 8 & 0x00FF;
    reg_BDL = cal_SBR & 0x00FF;

    //calc baud_rate
    reg_BDH = (cal_SBR & 0x1FFF) >> 8 & 0x00FF;
    reg_BDL = cal_SBR & 0x00FF;

    //calc bit_order
    if (cfg->bit_order == BIT_ORDER_LSB)
        reg_S2 &= ~(UART_S2_MSBF_MASK<<UART_S2_MSBF_SHIFT);
    else if (cfg->bit_order == BIT_ORDER_MSB)
        reg_S2 |= UART_S2_MSBF_MASK<<UART_S2_MSBF_SHIFT;

    //calc data_bits
    if (cfg->data_bits == DATA_BITS_8)
        reg_C1 &= ~(UART_C1_M_MASK<<UART_C1_M_SHIFT);
    else if (cfg->data_bits == DATA_BITS_9)
        reg_C1 |= UART_C1_M_MASK<<UART_C1_M_SHIFT;

    //clac parity
    if (cfg->parity == PARITY_NONE)
        reg_C1 &= ~(UART_C1_PE_MASK<<UART_C1_PE_SHIFT);
    else
    {
        reg_C1 &= ~(UART_C1_PE_MASK<<UART_C1_PE_SHIFT);
        
        if (cfg->parity == PARITY_ODD)
            reg_C1 |= UART_C1_PT_MASK<<UART_C1_PT_SHIFT;
        if (cfg->parity == PARITY_EVEN)
            reg_C1 &= ~(UART_C1_PT_MASK<<UART_C1_PT_SHIFT);
    }

    switch( (int)uart_reg)
    {
    case UART5_BASE:

        //set UART5 clock
        SIM->SCGC1 |= SIM_SCGC1_UART5_MASK;//Enable UART gate clocking 
        SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;//Enable PORTE gate clocking

        //set UART5 pin
        PORTE->PCR[ 8] = (3UL <<  8);      //Pin mux configured as ALT3
        PORTE->PCR[ 9] = (3UL <<  8);      //Pin mux configured as ALT3
        break;
    default:
        break;
    }

    uart_reg->BDH = reg_BDH;
    uart_reg->BDL = reg_BDL;
    uart_reg->C1 = reg_C1;
    uart_reg->S2 = reg_S2;

    uart_reg->S2  =  0;
    uart_reg->C3  =  0;

    uart_reg->RWFIFO = UART_RWFIFO_RXWATER(1);
    uart_reg->TWFIFO = UART_TWFIFO_TXWATER(0);

    uart_reg->C2  =  UART_C2_RE_MASK |    //Receiver enable
                     UART_C2_TE_MASK;     //Transmitter enable

    return RT_EOK;
}

static rt_err_t _control(struct rt_serial_device *serial, int cmd, void *arg)
{
    UART_Type *uart_reg;
    int uart_irq_num = 0;

    uart_reg = ((struct k60_serial_device *)serial->parent.user_data)->baseAddress;
    uart_irq_num = ((struct k60_serial_device *)serial->parent.user_data)->irq_num;

    switch (cmd)
    {
    case RT_DEVICE_CTRL_CLR_INT:
        /* disable rx irq */
        uart_reg->C2 &= ~UART_C2_RIE_MASK;
        //disable NVIC
        NVICICER1 |= 1 << (uart_irq_num % 32);
        break;
    case RT_DEVICE_CTRL_SET_INT:
        /* enable rx irq */
        uart_reg->C2 |= UART_C2_RIE_MASK;
        //enable NVIC,we are sure uart's NVIC vector is in NVICICPR1
        NVICICPR1 |= 1 << (uart_irq_num % 32);
        NVICISER1 |= 1 << (uart_irq_num % 32);
        break;
    case RT_DEVICE_CTRL_SUSPEND:
        /* suspend device */
        uart_reg->C2  &=  ~(UART_C2_RE_MASK |    //Receiver enable
                            UART_C2_TE_MASK);     //Transmitter enable
        break;
    case RT_DEVICE_CTRL_RESUME:
        /* resume device */
        uart_reg->C2  =  UART_C2_RE_MASK |    //Receiver enable
                         UART_C2_TE_MASK;     //Transmitter enable
        break;
    }

    return RT_EOK;
}

static int _putc(struct rt_serial_device *serial, char c)
{
    UART_Type *uart_reg;
    uart_reg = ((struct k60_serial_device *)serial->parent.user_data)->baseAddress;

    while (!(uart_reg->S1 & UART_S1_TDRE_MASK));
    uart_reg->D = (c & 0xFF);
    return 1;
}

static int _getc(struct rt_serial_device *serial)
{
    UART_Type *uart_reg;
    uart_reg = ((struct k60_serial_device *)serial->parent.user_data)->baseAddress;

    if (uart_reg->S1 & UART_S1_RDRF_MASK)
        return (uart_reg->D);
    else
        return -1;
}

static const struct rt_uart_ops _k60_ops =
{
    _configure,
    _control,
    _putc,
    _getc,
};


void UART5_RX_TX_IRQHandler(void)
{
    rt_hw_serial_isr((struct rt_serial_device*)&_k60_serial);
}


void rt_hw_uart_init(void)
{
    struct serial_configure config;

    /* fake configuration */
    config.baud_rate = BAUD_RATE_115200;
    config.bit_order = BIT_ORDER_LSB;
    config.data_bits = DATA_BITS_8;
    config.parity    = PARITY_NONE;
    config.stop_bits = STOP_BITS_1;
    config.invert    = NRZ_NORMAL;

    _k60_serial.ops    = &_k60_ops;
    _k60_serial.int_rx = &_k60_int_rx;
    _k60_serial.config = config;

    rt_hw_serial_register(&_k60_serial, "uart5",
                          RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_STREAM,
                          (void*)&_k60_node);

    rt_device_control(&_k60_serial.parent, RT_DEVICE_CTRL_SET_INT, 0);
}

void rt_hw_console_output(const char *str)
{
    while(*str != '\0')
    {
        if (*str == '\n')
            _putc(&_k60_serial,'\r');
        _putc(&_k60_serial,*str);
        str++;
    }
}
