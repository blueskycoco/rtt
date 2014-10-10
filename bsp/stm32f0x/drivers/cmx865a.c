#include <stm32f0xx.h>
#include "cmx865a.h"
SPI_InitTypeDef  SPI_InitStructure;
//rt_uint8_t send_data=0;
//rt_uint8_t recv_data=0;
unsigned char DTMF_TX_Busy = 0;//?????
unsigned char DTMF_TX_Num= 0;//????
unsigned char CMX865_RX_Flag;//????FSKorDTMF
unsigned char CID_RX_buff[max_buff];//?????
unsigned char CID_RX_count= 0;//???????
unsigned	int	temp_int;//?CMX865?????
unsigned char  CID_State=0;//????????
enum CID_recive_state//??FSK ?????
{
	Waite,
       Recived_55,
       Recived_02,
       Recived_long,
       Recived_num,
}CID_state=0;
rt_uint8_t phone_state;
void init_irq()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;


	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource1);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	EXTI_ClearITPendingBit(EXTI_Line1);
}
void init_spi()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the SPI periph */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	/* Enable SCK, MOSI, MISO and NSS GPIO clocks */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_0);//sck
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_0);//miso
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_0);//mosi
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_0);//cs

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;

	/* SPI SCK pin configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* SPI  MOSI pin configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* SPI MISO pin configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* SPI NSS pin configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* SPI configuration -------------------------------------------------------*/
	SPI_I2S_DeInit(SPI1);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
#if 0
	/* Configure the SPI interrupt priority */
	NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
	/* Initializes the SPI communication */
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_Init(SPI1, &SPI_InitStructure);

	/* Initialize the FIFO threshold */
	SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);

	/* Enable the Rx buffer not empty interrupt */
	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE);
	/* Enable the SPI Error interrupt */
	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_ERR, ENABLE);
	/* Data transfer is performed in the SPI interrupt routine */

	/* Enable the SPI peripheral */
	SPI_Cmd(SPI1, ENABLE);
}
#if 0
void write_spi(rt_uint8_t data)
{
	send_data=data;
	/* Enable the Tx buffer empty interrupt */
	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE, ENABLE);

	/* Waiting until TX FIFO is empty */
	while (SPI_GetTransmissionFIFOStatus(SPI1) != SPI_TransmissionFIFOStatus_Empty)
	{}

}
rt_uint8_t read_spi()
{
	/* Wait busy flag */
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
	{}

	/* Waiting until RX FIFO is empty */
	while (SPI_GetReceptionFIFOStatus(SPI1) != SPI_ReceptionFIFOStatus_Empty)
	{}
	
	return recv_data;
}

void SPI1_IRQHandler(void)
{

  /* SPI in Master Tramitter mode--------------------------------------- */
  if (SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_TXE) == SET)
  {
    
      SPI_SendData8(SPI1, send_data);
      SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE, DISABLE);
     
  }
  
  /* SPI in Master Receiver mode--------------------------------------- */
  if (SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_RXNE) == SET)
  {
    
      recv_data=SPI_ReceiveData8(SPI1);
      
  }
  
  /* SPI Error interrupt--------------------------------------- */
  if (SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_OVR) == SET)
  {
    SPI_ReceiveData8(SPI1);
    SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_OVR);
  }
}
#else
void write_cmx865a(rt_uint8_t addr,rt_uint16_t data,rt_uint8_t len)
{
	if(len==0)
		SPI_SendData8(SPI1, addr);
	else
		{
		SPI_SendData8(SPI1, addr);
		SPI_SendData8(SPI1, (data>>8) & 0xff);
		SPI_SendData8(SPI1, data&0xff);
		}
}
void read_cmx865a(rt_uint8_t addr,rt_uint8_t* data,rt_uint8_t len)
{
	rt_uint8_t i=0;
	SPI_SendData8(SPI1, addr);
	for(i=0;i<len;i++)
		data[len-i-1]=SPI_ReceiveData8(SPI1);
}

#endif
void cmx865a_isr(void)
{
	unsigned int  i; 
		unsigned char  j; 
		static unsigned char  k=0; 
		static unsigned char  fsk_long=0; 
		i =  Read_CMX865_AddrAndWord(Status_addr);
		if(CMX865_RX_Flag)
		{
			if(i&0x0020)//??DTMF
			{
				j=i&0x000f;
				if((j==M_or_P_key_value)||(j==Permit_Applay))
				{
					Rx_P_or_M=j;
				}
				else
				{
					if(CID_RX_count<max_buff)
					{
						if(j==key_0)
						{
							CID_RX_buff[CID_RX_count++]=0;
						}
						else if((j>0)&&(j<10))
						{
							CID_RX_buff[CID_RX_count++]=j;
						}
					}
				}
                    #if CID_Test
				Tx_char(j);
                    #endif
			}
			else
			{
				Read_CMX865_AddrAndByte(Receive_Data_addr);
				//Tx_char(Read_CMX865_AddrAndByte(Receive_Data_addr));
			}
		}
		else
		{
			if(i&0x0040)//??FSK
			{
				j=Read_CMX865_AddrAndByte(Receive_Data_addr);
				   if((j>='0')&&(j<='9'))
				   {
					    if(j==Permit_Num_Buff[CID_State])
					    {
							CID_State++;
							if(CID_State==Permit_Num_Buff[max_buff])
							{
								 // phone_state|=CID_Received;
							}
					    }
					    else
					    {
							CID_State=0;
					    }
				   }
				   else
				    {
					    CID_State=0;
				    }
                    #if 0
				switch(CID_state)
				{
					case Waite:
					{
						if(j==0x55)
						{
							k++;
							if(k>2)
							{
								k=0;
								CID_state=Recived_55;
								//Tx_char(1);
							}
						}
						else
						{
							k=0;
						}
						break;
					}
					case Recived_55:
					{
						if(j==0x02)
						{
							CID_state=Recived_02;
							//Tx_char(2);
						}
						else if(j==0x04)
						{
							CID_state=Recived_02;
						}
						break;
					}
					case Recived_02:
					{
						if(j<0x10)
						{
							fsk_long=j;
						}
						else
						{
							fsk_long=max_buff;
						}
						//Tx_char(fsk_long);
						CID_RX_count=0;
						CID_state=Recived_long;
						break;
					}	
					case Recived_long:
					{
						if(CID_RX_count<fsk_long)
						{
							CID_RX_buff[CID_RX_count++]=j-'0';
							/*
							if(CID_RX_count==max_buff)
							{
								CID_RX_count=max_buff-1;
							}
							*/
						}
						else
						{
							phone_state|=CID_Received;
							//Tx_char(255);
							CID_state=Waite;
						}
						break;
					}	
					default :
						break;
				}
                     #endif
			#if CID_Test
				Tx_char(j);
                    #endif
			}
		}

}

void cmx865a_init(void)
{
	rt_uint8_t data[2];
	phone_state=0;
	write_cmx865a(G_Reset_Command_addr,0,0);
	write_cmx865a(G_Control_Command_addr, Reset_CMX865|PowerUp,1);
	rt_thread_delay(50);
	write_cmx865a(G_Control_Command_addr, NORMAL,1);
	read_cmx865a(Status_addr,data,2);
	if(data[0]&0x00ff)
	{
		rt_kprintf("init cmx865a failed");
		return ;

	}
	else
	{	
		if(phone_state|DC_state)
		{
			temp_int = CMX865_Rx_Gain; 
			temp_int=temp_int<<9;
			if (1)
			{
				write_cmx865a(Receive_Mode_addr, Received_DTMF|temp_int,1);//????
				phone_state |= CID_Way;//??DTMF??
				rt_kprintf("DTMF Re");
			}
			else
			{
				write_cmx865a(Receive_Mode_addr, Received_FSK|temp_int,1);//????
				phone_state &=~ CID_Way;//??FSK??
				rt_kprintf("FSK Re");
			}
		}
		else//??????
		{
			write_cmx865a(G_Reset_Command_addr,0,0);
			write_cmx865a(G_Control_Command_addr, Reset_CMX865,1);
		}		
	}	
	return	1;
}
INIT_DEVICE_EXPORT(cmx865a_init);
