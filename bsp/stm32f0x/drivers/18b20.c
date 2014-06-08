
#include <stm32f0xx.h>
#include <rthw.h>
#include <rtthread.h>
#define DLY 10
//extern void Delay(__IO uint32_t nCount);
GPIO_InitTypeDef	GPIO_InitStructure;
#define  SkipROM    0xCC     //����ROM
#define  SearchROM  0xF0  //����ROM
#define  ReadROM    0x33  //��ROM
#define  MatchROM   0x55  //ƥ��ROM
#define  AlarmROM   0xEC  //�澯ROM
#define  StartConvert    0x44  //��ʼ�¶�ת�������¶�ת���ڼ����������0��ת�����������1
#define  ReadScratchpad  0xBE  //���ݴ�����9���ֽ�
#define  WriteScratchpad 0x4E  //д�ݴ������¶ȸ澯TH��TL
#define  CopyScratchpad  0x48  //���ݴ������¶ȸ澯���Ƶ�EEPROM���ڸ����ڼ����������0������������1
#define  RecallEEPROM    0xB8    //��EEPROM���¶ȸ澯���Ƶ��ݴ����У������ڼ����0��������ɺ����1
#define  ReadPower       0xB4    //����Դ�Ĺ��緽ʽ��0Ϊ������Դ���磻1Ϊ�ⲿ��Դ����
#define EnableINT()  
#define DisableINT()
#define DS_PORT   GPIOA
#define DS_DQIO   GPIO_Pin_1
#define DS_RCC_PORT  RCC_AHBPeriph_GPIOA
#define DS_PRECISION 0x7f   //�������üĴ��� 1f=9λ; 3f=10λ; 5f=11λ; 7f=12λ;
#define DS_AlarmTH  0x64
#define DS_AlarmTL  0x8a
#define DS_CONVERT_TICK 1000
#define ResetDQ() GPIO_ResetBits(DS_PORT,DS_DQIO)
#define SetDQ()  GPIO_SetBits(DS_PORT,DS_DQIO)
#define GetDQ()  GPIO_ReadInputDataBit(DS_PORT,DS_DQIO)
static unsigned char TempX_TAB[16]={0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x04,0x05,0x06,0x06,0x07,0x08,0x08,0x09,0x09};
void Delay_us(unsigned long Nus) 
{  
 /*SysTick->LOAD=Nus*9;          //ʱ�����       
 SysTick->CTRL|=0x01;             //��ʼ����     
 while(!(SysTick->CTRL&(1<<16))); //�ȴ�ʱ�䵽��  
 SysTick->CTRL=0X00000000;        //�رռ����� 
 SysTick->VAL=0X00000000;         //��ռ�����      
*/
	uint32_t ticks;
    uint32_t told,tnow,tcnt=0;
    uint32_t reload = SysTick->LOAD; /* LOAD��ֵ */
                 
    ticks = Nus * 100;                  /* ��Ҫ�Ľ����� */               
    tcnt = 0;
    told = SysTick->VAL;             /* �ս���ʱ�ļ�����ֵ */

    while(1)
    {
        tnow = SysTick->VAL;    
        if(tnow != told)
        {    
            /* ����ע��һ��SYSTICK��һ���ݼ��ļ����� */    
            if(tnow < told)
            {
                tcnt += told - tnow;    
            }
            /* ����װ�صݼ� */
            else 
            {
                tcnt += reload - tnow + told;    
            }        
            told = tnow;

            /*ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳� */
            if(tcnt >= ticks)break;
        }  
    }
} 
unsigned char ResetDS18B20(void)
{
 unsigned char resport;
 SetDQ();
 Delay_us(50);
 
 ResetDQ();
 Delay_us(500);  //500us ����ʱ���ʱ�䷶Χ���Դ�480��960΢�룩
 SetDQ();
 Delay_us(40);  //40us
 //resport = GetDQ();
 //rt_kprintf("ResetDS18B20 1\r\n");
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
GPIO_Init(GPIOA, &GPIO_InitStructure);	

 while(GetDQ());
 Delay_us(500);  //500us
 		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_Init(GPIOA, &GPIO_InitStructure);	

 SetDQ();
// rt_kprintf("ResetDS18B20 2\r\n");

 return resport;
}
void DS18B20WriteByte(unsigned char Dat)
{
 unsigned char i;
 for(i=8;i>0;i--)
 {
   ResetDQ();     //��15u���������������ϣ�DS18B20��15-60u����
  Delay_us(5);    //5us
  if(Dat & 0x01)
   SetDQ();
  else
   ResetDQ();
  Delay_us(65);    //65us
  SetDQ();
  Delay_us(2);    //������λ��Ӧ����1us
  Dat >>= 1; 
 } 
}
unsigned char DS18B20ReadByte(void)
{
 unsigned char i,Dat;
 SetDQ();
 Delay_us(5);
 for(i=8;i>0;i--)
 {
   Dat >>= 1;
    ResetDQ();     //�Ӷ�ʱ��ʼ�������ź��߱�����15u�ڣ��Ҳ�������������15u�����
  Delay_us(5);   //5us
  SetDQ();
  Delay_us(5);   //5us
  		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_Init(GPIOA, &GPIO_InitStructure);	

  if(GetDQ())
    Dat|=0x80;
  else
   Dat&=0x7f;  
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_Init(GPIOA, &GPIO_InitStructure);	

  Delay_us(65);   //65us
  SetDQ();
 }
 return Dat;
}
/*
void ReadRom(unsigned char *Read_Addr)
{
 unsigned char i;
 DS18B20WriteByte(ReadROM);
  
 for(i=8;i>0;i--)
 {
  *Read_Addr=DS18B20ReadByte();
  Read_Addr++;
 }
}*/
void DS18B20Init(unsigned char Precision,unsigned char AlarmTH,unsigned char AlarmTL)
{
 ResetDS18B20();
 DS18B20WriteByte(SkipROM); 
 DS18B20WriteByte(WriteScratchpad);
 DS18B20WriteByte(AlarmTL);
 DS18B20WriteByte(AlarmTH);
 DS18B20WriteByte(Precision);
 ResetDS18B20();
 DS18B20WriteByte(SkipROM); 
 DS18B20WriteByte(CopyScratchpad);
 		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_Init(GPIOA, &GPIO_InitStructure);	

 while(!GetDQ());  //�ȴ�������� ///////////
 		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_Init(GPIOA, &GPIO_InitStructure);	

}

void DS18B20StartConvert(void)
{
 ResetDS18B20();
 DS18B20WriteByte(SkipROM); 
 DS18B20WriteByte(StartConvert); 
}
void DS18B20_Configuration(void)
{
 //GPIO_InitTypeDef GPIO_InitStructure;
 
 RCC_AHBPeriphClockCmd(DS_RCC_PORT, ENABLE);
 GPIO_InitStructure.GPIO_Pin = DS_DQIO;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //��©���
 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //2Mʱ���ٶ�
 GPIO_Init(DS_PORT, &GPIO_InitStructure);
}

void ds18b20_start(void)
{
 DS18B20_Configuration();
 DS18B20Init(DS_PRECISION, DS_AlarmTH, DS_AlarmTL);
 DS18B20StartConvert();
}
unsigned short ds18b20_read(void)
{
 unsigned char TemperatureL,TemperatureH;
 unsigned int  Temperature;
 //rt_kprintf("read 0\r\n");
  ResetDS18B20();
 DS18B20WriteByte(SkipROM); 
 DS18B20WriteByte(ReadScratchpad);
 TemperatureL=DS18B20ReadByte();
 TemperatureH=DS18B20ReadByte(); 
 //rt_kprintf("read 1\r\n");
 ResetDS18B20();
 if(TemperatureH & 0x80)
  {
  TemperatureH=(~TemperatureH) | 0x08;
  TemperatureL=~TemperatureL+1;
  if(TemperatureL==0)
   TemperatureH+=1;
  }
 TemperatureH=(TemperatureH<<4)+((TemperatureL&0xf0)>>4);
 TemperatureL=TempX_TAB[TemperatureL&0x0f];
 //bit0-bit7ΪС��λ��bit8-bit14Ϊ����λ��bit15Ϊ����λ
 Temperature=TemperatureH;
 Temperature=(Temperature<<8) | TemperatureL; 
//rt_kprintf("read 2\r\n");
 DS18B20StartConvert();
 //rt_kprintf("read 3\r\n");
 return  Temperature;
}
#if 0
unsigned char read_18b20()
{
	unsigned char i,byte=0;
	for(i=8;i>0;i--)
	{
		byte=byte>>1;
		GPIO_ResetBits(GPIOA,GPIO_Pin_1);
		Delay(15);
		//GPIO_SetBits(GPIOA,GPIO_Pin_1);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_Init(GPIOA, &GPIO_InitStructure);	
		Delay(50);
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1))			
			byte=byte|0x80;
		else
			byte=byte&0x7f;
		Delay(600);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_Init(GPIOA, &GPIO_InitStructure);	

	}
	return byte;
}

void write_18b20(unsigned char byte)
{
	unsigned char i;
	for(i=8;i>0;i--)
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_1);
		Delay(125);
		if(byte&0x01)
		{
			GPIO_SetBits(GPIOA,GPIO_Pin_1);
			Delay(150);
		}
		else
		{
			Delay(150);
			GPIO_SetBits(GPIOA,GPIO_Pin_1);
		}
		byte=byte>>1;
		Delay(80);
		//GPIO_SetBits(GPIOA,GPIO_Pin_1);
	}
	//Delay(8*DLY);
}
void reset_18b20()
{
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
	Delay(10);
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	Delay(4000);
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)==Bit_SET);
	//rt_kprintf("x is %d\r\n",GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1));
	Delay(4000);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
}

unsigned int read_temp()
{
	int x=0;
	rt_base_t level;
	reset_18b20();
	write_18b20(0xcc);
	write_18b20(0x44);
	rt_thread_delay(10);
	
	reset_18b20();
	write_18b20(0xcc);
	write_18b20(0xbe);
	rt_kprintf("%x %x =>\r\n",read_18b20(),read_18b20());
	return x;
}
void init_18b20()
{
	/*1 reset 18b20*/		
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
#endif
INIT_DEVICE_EXPORT(ds18b20_start);

