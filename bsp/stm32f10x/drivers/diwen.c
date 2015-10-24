#include <rtthread.h>
#include "stm32f10x.h"
#include <math.h>
#include "usart.h"
#define UART_TYPE_LCD 0
#define UART_TYPE_PARAM 1
ALIGN(RT_ALIGN_SIZE)
static char lcd_stack[2048];
static char param_stack[2048];
long val_serial=0;
struct rt_semaphore rx_tp_sem;
struct rt_semaphore rx_param_sem;
char g_rx_param_buf[256];
int g_rx_param_len=0;
#define ALARM_INFO_PRESS 0x0001
#define ALARM_INFO_DATA	 0x0000
#define START_PRESS		 0x0002
#define START_DATA		 0x0001
#define S1_PRESS		 0x0003
#define S1_DATA			 0x0002
#define S2_PRESS		 0x0004
#define S2_DATA			 0x0003
#define S3_PRESS		 0x0005
#define S3_DATA			 0x0004
#define S4_PRESS		 0x0006
#define S4_DATA			 0x0005
#define S5_PRESS		 0x0007
#define S5_DATA			 0x0006
#define S6_PRESS		 0x0008
#define S6_DATA			 0x0007
#define S7_PRESS		 0x0009
#define S7_DATA			 0x0008
#define S8_PRESS		 0x000a
#define S8_DATA			 0x0010
#define VAR_TTR_TIME	 0x0101
#define VAR_TIME_SET	 0x0100
#define VAR_POWR_SET	 0x0102
#define VAR_CHANNEL_SET	 0x0103
#define VAR_DATE_TIME_1	 0x0104
#define VAR_DATE_TIME_2  0x0105
#define VAR_DATE_TIME_3	 0x0106
#define VAR_DATE_TIME_4	 0x0107
#define VAR_ALARM_TYPE_1 0x0108
#define VAR_ALARM_TYPE_2 0x0109
#define VAR_ALARM_TYPE_3 0x0110
#define VAR_ALARM_TYPE_4 0x0111
#define VAR_RUN_TIME	 0x0112
#define VAR_REAL_TIME_TEMP 0x0113
char g_rx_tp_buf[6];//first byte is pressd 1 or up 0 or invaled 2
unsigned short const SBtnGroup_XY[15][2][2]=
{//index=4
	86,183,164,242,//6
 	183,184,263,239,//7
 	279,184,360,240,//8
 	377,184,459,239,//9
 	477,183,557,239,//back space
 	85,281,165,336,// 2
 	183,280,263,335,// 3
 	281,282,360,337,// 4
 	378,280,459,337,// 5
 	476,280,557,336,//cleaer
 	85,377,167,435,//0
 	183,378,265,434,// 1
 	279,379,360,433,//+
 	379,378,458,434,//-
 	477,376,557,433//enter
};
unsigned short const SBmp2_XY[4][2][2]=
{//index=1
	54,206,304,253,//jie mianji
	335,209,586,253,//pinjun liusu
	54,389,304,433,// begin test
	336,389,585,433//safe power off
};
unsigned short const SBnum_L_XY[10][2][2]=
{//index=5or6
	229,227,247,254,// 0
	247,227,265,254,// 1
	265,227,283,254,// 2
	283,227,301,254,// 3
	301,227,319,254,// 4
	319,227,337,254,// 5
	337,227,355,254,// 6
	355,227,373,254,// 7
	373,227,391,254,// 8
	391,227,412,254// 9
};
unsigned short const SBnum_B_XY[10][2][2]=
{//index=7
	102,209,143,271,// 0
	143,209,184,271,// 1
	184,209,225,271,// 2
	225,209,266,271,// 3
	266,209,307,271,// 4
	//143,209,184,271,// 1
	//184,209,225,271,// 2
	
	307,209,348,271,// 5
	348,209,389,271,// 6
	389,209,430,271,// 7
	430,209,471,271,// 8
	471,209,517,271// 9
};
rt_err_t uart_param_rx_ind(rt_device_t dev, rt_size_t size)
{
	rt_sem_release(&rx_param_sem);
	//rt_kprintf("param uart rx ind\r\n");
	return RT_EOK;
}

rt_err_t uart_lcd_rx_ind(rt_device_t dev, rt_size_t size)
{
	rt_sem_release(&rx_tp_sem);
	//rt_kprintf("param lcd rx ind\r\n");
	return RT_EOK;
}
void uart_param_rx_ind_ex(void* parameter)
{
	char ch;
	int i=0;
	unsigned short x=0,y=0;
	rt_kprintf("param param rx ind ex\r\n");
	while(1)
	{
		//i=0;
		if (rt_sem_take(&rx_param_sem, RT_WAITING_FOREVER) != RT_EOK) continue;
		while (rt_device_read(uart_param_dev, 0, &ch, 1) == 1)
		{
			//rt_kprintf("=>%x \r\n",ch);
			i++;
			if(i==546)
				x=ch;
			if(i==547)
				x=(x<<8)|ch;				
			if(i==2024)
				y=ch;
			if(i==2025)
			{
				y=(y<<8)|ch;
				int val=y/x;
				int val1=(int)log(val);
				//if(val1>0)
					val_serial=ch;
				//rt_kprintf("val_serial %x %x %x got %x\r\n",y,x,val,val_serial);
				i=0;
				break;
			}
		}
		
	}
	return ;
}
#define STATE_ORIGIN 		0
#define STATE_MAIN 			1
#define STATE_ALARM_INFO 	2
#define STATE_START 		3
#define STATE_ALARM_WARNING 4
#define STATE_MAIN_S1		5
#define STATE_MAIN_S2		6
#define STATE_MAIN_S3		7
#define STATE_MAIN_S4		8
#define STATE_MAIN_S5		9
#define STATE_MAIN_S6		10
#define STATE_MAIN_S7		11
#define STATE_MAIN_S8		12

void uart_lcd_rx_ind_ex(void* parameter)
{
	rt_kprintf("lcd uart rx ind ex\r\n");
	char ch;
	int i=1;
	int get=0,type=2;//0 for up , 1 press , 2 for invaled
	rt_device_t dev=uart_lcd_dev;
	
	while(1)
	{
		if (rt_sem_take(&rx_tp_sem, RT_WAITING_FOREVER) != RT_EOK) continue;
		//g_rx_tp_buf[0]=2;
		while (rt_device_read(dev, 0, &ch, 1) == 1)
		{
		//rt_kprintf("<=%x \r\n",ch);
		switch(get)
		{
			case 0:
				if(ch==0x5a)
				{
					rt_kprintf("0x5a get ,get =1\r\n");
					get=1;
				}
				break;
			case 1:
				if(ch==0xa5)
				{
					rt_kprintf("0xa5 get ,get =2\r\n");
					get=2;
					
					}
				break;
			case 2:
				if(ch==0x06)
				{
					rt_kprintf("0x06 get,get =3\r\n");
					get=3;
					break;
				}
			case 3:
				if(ch==0x83)
				{
					rt_kprintf("0x83 get,get =4\r\n");
					get=4;
					i=1;
					break;
				}
			case 4:
				{
					rt_kprintf("%02x get ,get =5\r\n",ch);
					g_rx_tp_buf[i++]=ch;
					if(i==6)
					{
						get=0;
						g_rx_tp_buf[0]=0x01;
						rt_kprintf("get %x %x %x %x %x %x\r\n",g_rx_tp_buf[0],g_rx_tp_buf[1],g_rx_tp_buf[2],g_rx_tp_buf[3],g_rx_tp_buf[4],g_rx_tp_buf[5]);
					}
				}
				break;			
			default:
				rt_kprintf("unknown state\r\n");
				break;

					
		}
		
	}
		
		}
	return ;
}

unsigned short input_handle()
{
	int i=0,addr=0,data=0;
		while(g_rx_tp_buf[0]!=1)
			rt_thread_delay(20);
		rt_thread_delay(20);
		rt_kprintf("got press\r\n");
		g_rx_tp_buf[0]=2;
		addr=g_rx_tp_buf[1]<<8|g_rx_tp_buf[2];
		data=g_rx_tp_buf[4]<<8|g_rx_tp_buf[5];
		switch(addr)
		{
			
			case ALARM_INFO_PRESS:
				if(ALARM_INFO_DATA==data)
					return STATE_ALARM_INFO;
			case START_PRESS:
				if(START_DATA==data)
					return STATE_START;
			case S1_PRESS:
				if(S1_DATA==data)
					return STATE_MAIN_S1;
			case S2_PRESS:
			if(S2_DATA==data)
				return STATE_MAIN_S2;
			case S3_PRESS:
			if(S3_DATA==data)
				return STATE_MAIN_S3;
			case S4_PRESS:
			if(S4_DATA==data)
				return STATE_MAIN_S4;
			case S5_PRESS:
			if(S5_DATA==data)
				return STATE_MAIN_S5;
			case S6_PRESS:
			if(S6_DATA==data)
				return STATE_MAIN_S6;
			case S7_PRESS:
			if(S7_DATA==data)
				return STATE_MAIN_S7;
			case S8_PRESS:
			if(S8_DATA==data)
				return STATE_MAIN_S8;
			default:
				return STATE_MAIN;
			
		}
		
}

void switch_pic(unsigned char Index)	//??????????
{
	uart_tx(UART_TYPE_LCD,0x5a);
	uart_tx(UART_TYPE_LCD,0xa5);
	uart_tx(UART_TYPE_LCD,0x03);
	uart_tx(UART_TYPE_LCD,0x80);
	uart_tx(UART_TYPE_LCD,0x04);
	uart_tx(UART_TYPE_LCD,Index);
}
void write_data(unsigned int Index,int data)	//??????????
{
	uart_tx(UART_TYPE_LCD,0x5a);
	uart_tx(UART_TYPE_LCD,0xa5);
	uart_tx(UART_TYPE_LCD,0x05);
	uart_tx(UART_TYPE_LCD,0x82);
	uart_tx(UART_TYPE_LCD,(Index&0xff00)>>8);
	uart_tx(UART_TYPE_LCD,Index&0x00ff);
	uart_tx(UART_TYPE_LCD,(data&0xff00)>>8);
	uart_tx(UART_TYPE_LCD,data&0x00ff);
}
int get_touch()
{

}
void dgus_loop()
{
	int state=0,last_state=0,mianji_val_input=0,liushu_val_input=0;
	rt_err_t result;
	int offs=0;
	static struct rt_thread thread,thread1;
	state=STATE_ORIGIN;	
	last_state=STATE_ORIGIN;
	rt_sem_init(&rx_tp_sem, "shrx1", 0, 0);
	rt_sem_init(&rx_param_sem, "shrx2", 0, 0);

	result = rt_thread_init(&thread,
	    "ttfp",
	    uart_lcd_rx_ind_ex, RT_NULL,
	    &lcd_stack[0], sizeof(lcd_stack),
	    21, 10);

	if (result == RT_EOK)
	    rt_thread_startup(&thread);

	result = rt_thread_init(&thread1,
	    "tserial",
	    uart_param_rx_ind_ex, RT_NULL,
	    &param_stack[0], sizeof(param_stack),
	    22, 10);

	if (result == RT_EOK)
	    rt_thread_startup(&thread1);

	while(1)
	{
		//rt_kprintf("main_loop 6\r\n");
		switch(state)
			{
				case STATE_ORIGIN:
				{
					rt_kprintf("STATE_ORIGIN state , goto %d\r\n",state);
					switch_pic(0);
					offs++;
					//write_data(VAR_TTR_TIME,1223+offs);
					write_data(VAR_TIME_SET,2231+offs);
					write_data(VAR_POWR_SET,4433+offs);
					state=input_handle();
					last_state=STATE_ORIGIN;
					rt_kprintf("11STATE_ORIGIN state , goto %d\r\n",state);
					break;
				}
				case STATE_MAIN:
				{
					rt_kprintf("STATE_MAIN , last state %d\r\n",last_state);	
					
					//write_data(VAR_TTR_TIME,1223+offs);
					write_data(VAR_TIME_SET,2231+offs);
					write_data(VAR_POWR_SET,4433+offs);
					state=input_handle();
					last_state=STATE_MAIN;
					offs++;
					break;
				}			
				case STATE_ALARM_INFO:
				{
					rt_kprintf("STATE_ALARM_INFO , last state %d\r\n",last_state);	
					write_data(VAR_DATE_TIME_1,7832+offs);
					write_data(VAR_DATE_TIME_2,7635+offs);
					write_data(VAR_DATE_TIME_3,9037+offs);
					write_data(VAR_DATE_TIME_4,7362+offs);
					write_data(VAR_ALARM_TYPE_1,747736+offs);
					write_data(VAR_ALARM_TYPE_2,747636+offs);
					write_data(VAR_ALARM_TYPE_3,35+offs);
					write_data(VAR_ALARM_TYPE_4,746+offs);
					input_handle();
					state=STATE_MAIN;
					last_state=STATE_MAIN;
					offs++;
					break;
				}		
				case STATE_START:
				{
					rt_kprintf("STATE_START , last state %d\r\n",last_state);	
					write_data(VAR_RUN_TIME,1122+offs);
					write_data(VAR_REAL_TIME_TEMP,3321+offs);
					input_handle();
					state=STATE_MAIN;
					last_state=STATE_MAIN;
					offs++;
					break;
				}	
				case STATE_MAIN_S1:
				case STATE_MAIN_S2:
				case STATE_MAIN_S3:
				case STATE_MAIN_S4:
				case STATE_MAIN_S5:
				case STATE_MAIN_S6:
				case STATE_MAIN_S7:
				case STATE_MAIN_S8:
				{
					write_data(VAR_CHANNEL_SET,state-4);
				}
				default:					
				{
					state=STATE_ORIGIN;
					last_state=STATE_ORIGIN;
					rt_kprintf("UNKNOWN  state , got STATE_ORIGIN\r\n");
					break;
				}
			}
		
	}
}

