#include <stm32f0xx.h>
#include "cmx865a.h"
SPI_InitTypeDef  SPI_InitStructure;
rt_uint8_t send_data=0;
rt_uint8_t recv_data=0;
#define max_buff 256
unsigned char DTMF_TX_Busy = 0;
unsigned char DTMF_TX_Num= 0;
unsigned char CMX865_RX_Flag;
unsigned char CID_RX_buff[max_buff];
unsigned char CID_RX_count= 0;
unsigned	int	temp_int=6;
unsigned char  CID_State=0;
#define DTMF_MODE 0
#define key_0 'D'
#define CID_Received 1
enum CID_recive_state
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


	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOF, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOF, GPIO_Pin_1);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource1);
	//SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, EXTI_PinSource1);
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
	init_irq();
	
	/* Enable SCK, MOSI, MISO and NSS GPIO clocks */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	//GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_0);//sck	
	//GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_0);//miso	
	//GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_0);//mosi

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;

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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* SPI NSS pin configuration */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
void ms_delay()
{
	volatile int i,j;
	for(i=0;i<120;i++)
		j=0;
		
}
rt_uint8_t write_spi(rt_uint8_t data)
{
	
	rt_uint8_t i; 
	rt_uint8_t Temp=0x00;
	unsigned char SDI; 
	//GPIO_ResetBits(GPIOA, GPIO_Pin_5);
	for (i = 0; i < 8; i++)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_5);//sclk = 0;//先将时钟拉低
		ms_delay();
		if (data&0x80)      
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_7); //    //mosi=1 
		}
		else
		{
			GPIO_ResetBits(GPIOA, GPIO_Pin_7);//     //smosi=0
		}
		data <<= 1;  
		GPIO_ResetBits(GPIOA, GPIO_Pin_5);//    //sclk = 1; 拉高时钟
		ms_delay();
		SDI = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);//判断miso是否有输出
		Temp<<=1;

		if(SDI)       //读到1时
		{
			Temp++;  //置1  即向右移动一位同时加1   因上边有<<=1
		}
		GPIO_SetBits(GPIOA, GPIO_Pin_5);//sclk = 0;//   拉低时钟 
	}
	//GPIO_SetBits(GPIOA, GPIO_Pin_4);
	return Temp; //返回读到miso输入的值     
}
void write_cmx865a(rt_uint8_t addr,rt_uint16_t data,rt_uint8_t len)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
	if(len==0)
		write_spi(addr);
	else
		{
		write_spi(addr);
		if(len==2)
			write_spi((data>>8) & 0xff);
		write_spi(data&0xff);
		}
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
}
void read_cmx865a(rt_uint8_t addr,rt_uint8_t* data,rt_uint8_t len)
{

	rt_uint8_t i=0;
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
	write_spi(addr);
	//write_spi(0);	
	data[0]=write_spi(0);
	if(len==2){	
		//write_spi(0);
		data[1]=data[0];
		data[0]=write_spi(0);
		}
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
}
void button_isr(void)
{
	/*rt_kprintf("button_isr intr\r\n");
	
	write_cmx865a(Transmit_Mode_addr, Transmit_DTMF|0x8,2);
	write_cmx865a(Transmit_Mode_addr, Transmit_DTMF|0x6,2);
	write_cmx865a(Transmit_Mode_addr, Transmit_DTMF|0x5,2);
	write_cmx865a(Transmit_Mode_addr, Transmit_DTMF|0x4,2);
	write_cmx865a(Transmit_Mode_addr, Transmit_DTMF|0x3,2);
	write_cmx865a(Transmit_Mode_addr, Transmit_DTMF|0x5,2);
	write_cmx865a(Transmit_Mode_addr, Transmit_DTMF|0x3,2);
	write_cmx865a(Transmit_Mode_addr, Transmit_DTMF|0x1,2);*/
	
}
void cmx865a_isr(void)
{
	unsigned int  i,tmp; 
	unsigned char  j; 
	static unsigned char  k=0; 
	static unsigned char  fsk_long=0; 
	read_cmx865a(Status_addr,&i,2);
	rt_kprintf("status %x\r\n",i);
	if(DTMF_MODE)
	{
		if(i&0x0020)//??DTMF
		{
			j=i&0x000f;
			/*if((j==M_or_P_key_value)||(j==Permit_Applay))
			{
				Rx_P_or_M=j;
			}
			else
			{*/
				if(CID_RX_count<max_buff)
				{
					if(j==key_0)
					{
						CID_RX_buff[CID_RX_count++]=0;
					}
					else if((j>0)&&(j<10))
					{
						CID_RX_buff[CID_RX_count++]=j;
						rt_kprintf("Got DTMF Num %d %c\r\n",j,j);
					}
				}
			//}
		}
		else
		{
			read_cmx865a(Receive_Data_addr,&tmp,2);
		}
	}
	else
	{
		if(i&0x0040)//??FSK
		{
			read_cmx865a(Receive_Data_addr,&j,2);
			if((j>='0')&&(j<='9'))
			{
				/*if(j==Permit_Num_Buff[CID_State])
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
				}*/
			}
			else
			{
				CID_State=0;
			}
		//	rt_kprintf("==> %d %x\r\n",j,j);
		//	if(j>='0'&&j<='9')
			//	rt_kprintf(">>%c\r\n",j);
#if 1
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
				rt_kprintf("Got FSK Num %d %c\r\n",j,j);
			}
			else
			{
				phone_state|=CID_Received;
				CID_state=Waite;
				GPIO_SetBits(GPIOF, GPIO_Pin_1);
				rt_kprintf("finish receive phone num\r\n");
			}
			break;
		}	
		default :
		break;
	}
#endif
		}
	}

}

void test_cmx865a()
{
	rt_uint16_t data;
	static int flag=1;
	//while(1){
		read_cmx865a(Status_addr,&data,2);
		rt_kprintf("4 cmx865a_init status %x\r\n",data);
		rt_thread_delay(5);
		data=0;
		//write_cmx865a(Transmit_Data_addr,data,1);
		//rt_kprintf("cmx865a_init tx data %x\r\n",data);
		read_cmx865a(Receive_Data_addr,&data,1);
		rt_kprintf("4 cmx865a_init rx data %x\r\n\r\n",data);
		//if(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_1)==Bit_RESET && flag){
		//	GPIO_SetBits(GPIOA, GPIO_Pin_9);
		//	rt_kprintf("New call in\r\n");
		//	flag=0;
		//	}
	//	rt_thread_delay(100);
	//	}

}
void cmx865a_init(void)
{
	rt_uint16_t data;
	phone_state=0;
	init_spi();
	write_cmx865a(G_Reset_Command_addr,0,0);
	rt_thread_delay(5);
	write_cmx865a(G_Control_Command_addr, Reset_CMX865|PowerUp,2);
	rt_thread_delay(50);
	write_cmx865a(G_Control_Command_addr, NORMAL,2);

	read_cmx865a(Status_addr,&data,2);
	rt_kprintf("cmx865a_init %x\r\n",data);
	if(data&0x00ff)
	{
		rt_kprintf("init cmx865a failed");
		return ;

	}
	else
	{	
	//	if(phone_state|DC_state)
	//	{
			//temp_int = CMX865_Rx_Gain; 
			temp_int=temp_int<<9;
			if (DTMF_MODE)
			{
				write_cmx865a(Receive_Mode_addr, Received_DTMF|temp_int,2);//????
			//	phone_state |= CID_Way;//??DTMF??
				rt_kprintf("DTMF Re");
			}
			else
			{
				write_cmx865a(Receive_Mode_addr, Received_FSK|temp_int,2);//????
			//	phone_state &=~ CID_Way;//??FSK??
				rt_kprintf("FSK Re");
			}
		//}
		//else//??????
		//{
		//	write_cmx865a(G_Reset_Command_addr,0,0);
		//	write_cmx865a(G_Control_Command_addr, Reset_CMX865,1);
	//	}		
	}	
	return ;
}
INIT_DEVICE_EXPORT(cmx865a_init);
