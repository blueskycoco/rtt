#define max_buff 256
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
#define	G_Reset_Command_addr	0x01
#define	G_Control_Command_addr	0xe0
#define	Transmit_Mode_addr	0xe1
#define	Receive_Mode_addr		0xe2
#define	Transmit_Data_addr	0xe3
#define	Receive_Data_addr		0xe5
#define	Status_addr			0xe6
#define	Programming_addr		0xe8

#define	NORMAL			0x8141//8141/815f
#define	TXAOutDisable		0x4000
#define     AnalogueLoopbackTest  	0x0800
#define     equalizer     		0x0400
#define	PowerUp			0x0100//NORMAL&0xfeff
#define	Reset_CMX865		0x0080
#define	IRQ_EN			0x0040

#define	Transmit_disable		0x0000//0 001 111  0  000 10110 /(00000)
#define	Transmit_DTMF		0x1810// 1e10
#define	Received_DTMF		0x1001//0001 
#define	Received_FSK		0x5035//5e37
#define	DTMF_0			0x1a
#define	DTMF_1			0x11
#define	DTMF_2			0x12
#define	DTMF_3			0x13
#define	DTMF_4			0x14
#define	DTMF_5			0x15
#define	DTMF_6			0x16
#define	DTMF_7			0x17
#define	DTMF_8			0x18
#define	DTMF_9			0x19
#define	DTMF_D			0x00
#define	DTMF_X			0x1b
#define	DTMF_J			0x1c
#define	DTMF_A			0x1d
#define	DTMF_B			0x1e
#define	DTMF_C			0x1f

rt_uint8_t phone_state;
#define CLK_PIN 141
#define MOSI_PIN 142
#define MISO_PIN 143
#define CS_PIN 144
void ms_delay()
{
	volatile int i,j;
	for(i=0;i<10;i++)
		j=0;
}
void CLK(bool ctl)
{
	if(ctl)
		gpio_direction_output(CLK_PIN, 1);
	else
		gpio_direction_output(CLK_PIN, 0);
}
void MOSI(bool ctl)
{
	if(ctl)
		gpio_direction_output(MOSI_PIN, 1);
	else
		gpio_direction_output(MOSI_PIN, 0);
}
void CS(bool ctl)
{
	if(ctl)
		gpio_direction_output(CS_PIN, 1);
	else
		gpio_direction_output(CS_PIN, 0);
}
unsigned char MISO()
{
	return gpio_get_value(MISO_PIN);
}
rt_uint8_t write_spi(rt_uint8_t data)
{
	
	rt_uint8_t i; 
	rt_uint8_t Temp=0x00;
	unsigned char SDI; 
	for (i = 0; i < 8; i++)
	{
		CLK(1);
		ms_delay();
		if (data&0x80)      
		{
			MOSI(1);
		}
		else
		{
			MOSI(0);
		}
		data <<= 1;  
		CLK(0);
		ms_delay();
		SDI = MISO();
		Temp<<=1;

		if(SDI)
		{
			Temp++;
		}
		CLK(1);
	}
	
	return Temp; 
}
void write_cmx865a(rt_uint8_t addr,rt_uint16_t data,rt_uint8_t len)
{
	CS(0);
	if(len==0)
		write_spi(addr);
	else
		{
		write_spi(addr);
		if(len==2)
			write_spi((data>>8) & 0xff);
		write_spi(data&0xff);
		}
	CS(1);
}
void read_cmx865a(rt_uint8_t addr,rt_uint8_t* data,rt_uint8_t len)
{

	rt_uint8_t i=0;
	CS(0);
	write_spi(addr);
	data[0]=write_spi(0);
	if(len==2)
	{	
		data[1]=data[0];
		data[0]=write_spi(0);
	}
	CS(1);
}
void cmx865a_isr(void)
{
	unsigned int  i,tmp; 
	unsigned char  j; 
	static unsigned char  k=0; 
	static unsigned char  fsk_long=0; 
	read_cmx865a(Status_addr,&i,2);
	
	if(DTMF_MODE)
	{
		if(i&0x0020)//DTMF
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
						printk("Got DTMF Num %d %c\r\n",j,j);
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
		if(i&0x0040)//FSK
		{
			read_cmx865a(Receive_Data_addr,&j,2);
			
		//	rt_kprintf("==> %d %x\r\n",j,j);
			if(j>='0'&&j<='9')
				printk(">>%c\r\n",j);
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
					printk("Got FSK Num %d %c\r\n",j,j);
				}
				else
				{
					CID_state=Waite;
					printk("finish receive phone num\r\n");
				}
				break;
			}	
			default:
				break;
			}
		}
	}
}
void cmx865a_init(void)
{
	rt_uint16_t data;
	write_cmx865a(G_Reset_Command_addr,0,0);
	rt_thread_delay(5);
	write_cmx865a(G_Control_Command_addr, Reset_CMX865|PowerUp,2);
	rt_thread_delay(50);
	write_cmx865a(G_Control_Command_addr, NORMAL,2);

	read_cmx865a(Status_addr,&data,2);
	if(data&0x00ff)
	{
		printk("init cmx865a failed");
		return ;

	}
	else
	{	
		temp_int=temp_int<<9;
		if (DTMF_MODE)
		{
			write_cmx865a(Receive_Mode_addr, Received_DTMF|temp_int,2);//????
			printk("DTMF Re");
		}
		else
		{
			write_cmx865a(Receive_Mode_addr, Received_FSK|temp_int,2);//????
			printk("FSK Re");
		}
	}	
	return ;
}

