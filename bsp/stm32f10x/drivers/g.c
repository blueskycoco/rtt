/*
 *	brife: this is at88sc driver code,gpio code 
 */
#include "s.h"
#include <rthw.h>
#include <stm32f10x.h>

GPIO_InitTypeDef  GPIO_InitStructure;
void delay_us(int us)
{
	volatile unsigned long delay = 0;
	for (delay = 0; delay < us; )
	{
		delay++;
	}
}
void sleep_ms(unsigned long n)
{
	volatile long i,j;
	for(i=0;i<n;i++)
	for(j=0;j<10;j++)
	;
}

void i2c_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
void i2c_scl_set(unsigned char level)
{
	if(level)
		GPIO_SetBits(GPIOA, GPIO_Pin_10);
	else
		GPIO_ResetBits(GPIOA, GPIO_Pin_10);
	delay_us(10);
}
void i2c_sda_set(unsigned char level)
{
	if(level)
		GPIO_SetBits(GPIOA, GPIO_Pin_9);
	else
		GPIO_ResetBits(GPIOA, GPIO_Pin_9);
	delay_us(10);
}
unsigned char i2c_sda_get(void)
{
	return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_9);
}
void i2c_sda_input(void)
{
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	delay_us(10);
}
void i2c_sda_output(void)
{
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
void i2c_SendStart(void)
{
	i2c_sda_set(1);
	i2c_scl_set(1);
	i2c_sda_set(0);
	i2c_scl_set(0);
}
void i2c_SendStop(void)
{
	i2c_sda_set(0);
	i2c_scl_set(1);
	i2c_sda_set(1);
}
void i2c_SendAck(void)
{
	i2c_sda_set(0);
	i2c_scl_set(0);
	i2c_scl_set(1);
	i2c_scl_set(0);
}
void i2c_SendNack(void)
{
	i2c_sda_set(1);
	i2c_scl_set(0);
	i2c_scl_set(1);
	i2c_scl_set(0);
}
void i2c_SendData(unsigned char data)
{
	unsigned char bit;

	for (bit = 0x80; bit != 0; bit >>= 1)
	{   
		if (data & bit)
			i2c_sda_set(1);
		else
			i2c_sda_set(0);
		i2c_scl_set(1);
		i2c_scl_set(0);		
	}
}
unsigned char i2c_ReceiveAck(void)
{
	unsigned char bit;

	i2c_sda_input();
	i2c_scl_set(1);
	bit = i2c_sda_get();
	i2c_scl_set(0);
	i2c_sda_output();

	return !bit;
}
unsigned char i2c_ReceiveData(void)
{
	unsigned char data = 0;
	unsigned char x;

	i2c_sda_input();
	for (x = 0; x < 8; x++)
	{   
		i2c_scl_set(1);
		data <<= 1;
		data |= i2c_sda_get();
		i2c_scl_set(0);
	}
	i2c_sda_output();
	return data;                        
}
/*cm_PowerOn the first lowlevel function will be called*/
void cm_PowerOn(void)
{
	static BOOL flag=FALSE;
	int i=0;
	if(flag==FALSE)
	{
		/* init SIO low-level abstraction layer */
		i2c_init();
		/* set the clock and data lines to the proper states */
		i2c_scl_set(1);
		i2c_sda_set(1);
		sleep_ms(10);
		for(i=0;i<6;i++)
		{
			i2c_scl_set(1);
			sleep_ms(1);
			i2c_scl_set(0);
			sleep_ms(1);
		}
		i2c_scl_set(1);
		flag=TRUE;
	}
}
BOOL cm_Read(unsigned char Command, unsigned char Addr1, unsigned char Addr2,unsigned char Nbytes,unsigned char *pBuffer)
{
	unsigned char i;
	int restart_count = 0;

	unsigned char *pReadPtr = pBuffer;

	if (!pBuffer)
	{	    
		return FALSE;
	}		

	if (Nbytes > BYTES_MAX)
	{
		AT88DBG("<cm_Read>Warning: read too many bytes (%d) a time!\r\n", Nbytes);
		return FALSE;
	}

	while (restart_count++ < 5)
	{
		/* If restart, revert to the start of buffer! */
		pReadPtr = pBuffer;
		i2c_SendStart();
		i2c_SendData(Command);
		if (!i2c_ReceiveAck())
		{
			AT88DBG("<cm_Read>NACK received after Command.\n");
			i2c_SendStop();
			sleep_ms(20UL);
			continue;
		}
		i2c_SendData(Addr1);
		if (!i2c_ReceiveAck())
		{
			AT88DBG("<cm_Read>NACK received after Addr1.\n");
			i2c_SendStop();
			sleep_ms(20UL);
			continue;
		}
		i2c_SendData(Addr2);
		if (!i2c_ReceiveAck())
		{
			AT88DBG("<cm_Read>NACK received after Addr2.\n");
			i2c_SendStop();
			sleep_ms(20UL);
			continue;
		}	
		i2c_SendData(Nbytes);
		if (!i2c_ReceiveAck())
		{
			AT88DBG("<cm_Read>NACK received after Nbytes.\n");
			i2c_SendStop();
			sleep_ms(20UL);
			continue;
		}
		for (i = 0; i < Nbytes; i++)
		{		   	
			*pReadPtr++ = i2c_ReceiveData();       
			if(i<(Nbytes-1))
				i2c_SendAck();        	        
		}

		i2c_SendNack();
		i2c_SendStop();
		return TRUE;
	}

	AT88DBG("<cm_Read>(0x%x) failed, too many NACKs.\r\n", Command);	
	return FALSE;
}

unsigned int cm_Write(unsigned char Command, unsigned char Addr1, unsigned char Addr2,unsigned char Nbytes,unsigned char *pBuffer)
{
	unsigned long i;
	int restart_count = 0;
	unsigned char *pWritePtr = pBuffer;

	if (Nbytes > BYTES_MAX)
	{
		AT88DBG("<cm_Write>Error: write too many bytes (%d) a time!\r\n", Nbytes);
		return 0;
	}

	while (restart_count++ < 5)
	{
		if(pBuffer !=NULL && Nbytes != 0)
			pWritePtr = pBuffer;

		i2c_SendStart();
		i2c_SendData(Command);
		if (!i2c_ReceiveAck())
		{
			AT88DBG("<cm_Write>NACK received after Command. %x\r\n",Command);
			i2c_SendStop();
			sleep_ms(20UL);
			continue;
		}
		i2c_SendData(Addr1);
		if (!i2c_ReceiveAck())
		{
			AT88DBG("<cm_Write>NACK received after Addr1.\r\n");
			i2c_SendStop();
			sleep_ms(20UL);
			continue;
		}
		i2c_SendData(Addr2);
		if (!i2c_ReceiveAck())
		{
			AT88DBG("<cm_Write>NACK received after Addr2.\r\n");
			i2c_SendStop();
			sleep_ms(20UL);
			continue;
		}
		i2c_SendData(Nbytes);
		if (!i2c_ReceiveAck())
		{
			AT88DBG("<cm_Write>NACK received after Nbytes.\r\n");
			i2c_SendStop();
			sleep_ms(20UL);
			continue;
		}
		/* Following bytes are written in successive internal registers */
		if(pBuffer !=NULL && Nbytes != 0)
		{
			for (i = 0; i < Nbytes; i++)
			{
				i2c_SendData(*pWritePtr++);
				if (!i2c_ReceiveAck())
				{
					AT88DBG("<cm_Write>NACK received after %x data byte.\r\n", i+1);
					i2c_SendStop();
					sleep_ms(20UL);
					break;
				}            
			}
			if(i<Nbytes)
			{
				continue;
			}
		}

		/* done. */
		i2c_SendStop();

		return Nbytes;

	}

	AT88DBG("<cm_Write>(0x%x) failed, too many NACKs.\r\n", Addr2);
	return 0;
}
void cm_AckPolling(unsigned char Command)
{
	//Acknowledge Polling
	i2c_SendStart();
	/* Write device address */
	i2c_SendData(Command);
	while (!i2c_ReceiveAck())
	{
		//AT88DBG("<cm_AckPolling>Acknowledge Polling\r\n");
		i2c_SendStart();
		i2c_SendData(Command);
		sleep_ms(2UL);        
	}
	i2c_SendStop();
}

