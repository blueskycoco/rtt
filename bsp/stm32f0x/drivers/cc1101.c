#include <rthw.h>
#include <rtthread.h>
#include <stm32f0xx.h>
#include <rtdevice.h>
#include "cc1101.h"
#define GDO0_H (1<<0)
#define GDO0_L (1<<1)
struct rt_event cc1101_event;
void spi_init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    /* Enable the SPI peripheral */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    /* Enable SCK, MOSI, MISO and NSS GPIO clocks */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOC, ENABLE);

    /* SPI pin mappings */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_0);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_0);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_0);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_0);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;

    /* SPI SCK pin configuration */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* SPI  MOSI pin configuration */
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* SPI MISO pin configuration */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* SPI NSS pin configuration
     * */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
#if 0
    /* cc1101 int init
     * */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* Configure the SPI interrupt priority */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Clear DM9000A EXTI line pending bit */
    EXTI_ClearITPendingBit(EXTI_Line4);
#endif
    /* SPI configuration
     * -------------------------------------------------------*/
    SPI_I2S_DeInit(SPI1);
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;

    /* Initializes the SPI communication */
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_Init(SPI1, &(SPI_InitStructure));

    /* Initialize the FIFO threshold */
    SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);

    /* Enable NSS output for master mode */
    SPI_SSOutputCmd(SPI1, ENABLE);

}
int check_status(uint8_t bit)
{
	int i=0;
	while(SPI_I2S_GetITStatus(SPI1,bit)!=SET)
	{
		i++;
		if(i==100){
			DEBUG("check bit %x timeout\r\n",bit);
			return RT_FALSE;
			}
		rt_thread_delay(50);
	}
	return RT_TRUE;
}
uint8_t spi_send_rcv(uint8_t *data,int len)
{
	int i=0;
	uint8_t r=0;
	/* Enable the SPI peripheral */
	SPI_Cmd(SPI1, ENABLE);
	rt_thread_delay(2);
	
	for(i=0;i<len;i++)
	{
		if(check_status(SPI_I2S_IT_TXE))
			SPI_SendData8(SPI1, data[i]);
		if(check_status(SPI_I2S_IT_RXNE))
			r=SPI_ReceiveData8(SPI1);

	}

	/* Disable the SPI peripheral */
	 SPI_Cmd(SPI1, DISABLE);

	return r;
}

void reset_cs()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
DEBUG("cc1101 init qq1\r\n");
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);
DEBUG("cc1101 init qq2\r\n");
	rt_thread_delay(2);
DEBUG("cc1101 init qq3\r\n");
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET);
	DEBUG("cc1101 init qq4\r\n");
	rt_thread_delay(2);
DEBUG("cc1101 init qq5\r\n");
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);
	DEBUG("cc1101 init qq6\r\n");
	rt_thread_delay(10);
DEBUG("cc1101 init qq7\r\n");
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_0);
	DEBUG("cc1101 init qq8\r\n");
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
DEBUG("cc1101 init qq9\r\n");
	return;
}
int cc1101_init()
{
    rt_bool_t status = RT_FALSE;
	 DEBUG("cc1101 init 1\r\n");
	rt_event_init(&cc1101_event, "cc1101_event", RT_IPC_FLAG_FIFO );
	DEBUG("cc1101 init 2\r\n");
	spi_init();
	DEBUG("cc1101 init 3\r\n");
	cc1101_hw_init();
	DEBUG("cc1101 init 4\r\n");
	return RT_TRUE;
}

void cc1101_isr()
{
	if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4) ==SET)
		rt_event_send(&cc1101_event,GDO0_H);
	else
		rt_event_send(&cc1101_event,GDO0_L);
}

int wait_int(int flag)
{
	rt_uint32_t ev;
	if(flag)
		{
			/*wait for gdo0 to h */
			if( rt_event_recv( &cc1101_event, GDO0_H, RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, 1000, &ev ) != RT_EOK ) 
			{
				rt_kprintf("wait for h failed\r\n");
				return RT_FALSE;
			}
		}
	else
		{
			/*wait for gdo0 to l */
			if( rt_event_recv( &cc1101_event, GDO0_L, RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, 1000, &ev ) != RT_EOK ) 
			{
				rt_kprintf("wait for l failed\r\n");
				return RT_FALSE;
			}
		}
	return RT_TRUE;
}

void cc1101_send(uint8_t *buf,uint8_t len)
{
	return cc1101_send_packet(buf,len);
}

uint8_t cc1101_recv(uint8_t *buf,uint8_t len)
{
	uint8_t *len1;
	*len1=len;
	cc1101_rcv_packet(buf, len1);
	return *len1;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT_ALIAS(cc1101_send, csend,send data by cc1101);
FINSH_FUNCTION_EXPORT_ALIAS(cc1101_recv, crcv,recv data by cc1101);
FINSH_FUNCTION_EXPORT_ALIAS(cc1101_init, cinit,init cc1101);
#endif

