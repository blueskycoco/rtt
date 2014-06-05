
#include <stm32f0xx.h>
extern void Delay(__IO uint32_t nCount);

unsigned char read_18b20()
{
	unsigned char i,byte=0;
	for(i=8;i>0;i--)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_1);
		Delay(10);
		GPIO_ResetBits(GPIOA,GPIO_Pin_1);
		Delay(20);		
		GPIO_SetBits(GPIOA,GPIO_Pin_1);		
		Delay(20);
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1))			
			byte=byte|0x80;
		byte=byte>>1;
		Delay(200);
	}
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
	return byte;
}

void write_18b20(unsigned char byte)
{
	unsigned char i;
	for(i=8;i>0;i--)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_1);
		Delay(10);
		GPIO_ResetBits(GPIOA,GPIO_Pin_1);
		Delay(20);
		if(byte&0x01)
			GPIO_SetBits(GPIOA,GPIO_Pin_1);
		else
			GPIO_ResetBits(GPIOA,GPIO_Pin_1);
		Delay(200);
		byte=byte>>1;
	}
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
	Delay(30);
}
void reset_18b20()
{
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	Delay(1000);
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
	while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)==Bit_SET);
	Delay(1000);
}

unsigned int read_temp()
{
	reset_18b20();
	Delay(10);
	write_18b20(0xcc);
	write_18b20(0x44);
	reset_18b20();
	Delay(10);
	write_18b20(0xcc);
	write_18b20(0xbe);
	return ((read_18b20()&0x0f)<<8|read_18b20());
}
void init_18b20()
{
	/*1 reset 18b20*/	
	GPIO_InitTypeDef	GPIO_InitStructure;
	
	/* Enable the GPIO_LED Clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	
	/* Configure the GPIO_LED pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
}

