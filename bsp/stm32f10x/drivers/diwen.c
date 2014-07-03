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

char g_rx_tp_buf[5];//first byte is pressd 1 or up 0 or invaled 2
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
	//225,209,266,271,// 3
	//266,209,307,271,// 4
	143,209,184,271,// 1
	184,209,225,271,// 2
	
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

void uart_lcd_rx_ind_ex(void* parameter)
{
	rt_kprintf("lcd uart rx ind ex\r\n");
	char ch;
	int i=1;
	int get=0,type=2;//0 for up , 1 press , 2 for invaled
	rt_device_t dev=uart_lcd_dev;
	
	while(1)
	{
		//get=0;
		//i=1;
	//	g_rx_tp_buf[0]=2;
	//	rt_kprintf("lcd rx ind restart\r\n");
	if (rt_sem_take(&rx_tp_sem, RT_WAITING_FOREVER) != RT_EOK) continue;
	g_rx_tp_buf[0]=2;
	while (rt_device_read(dev, 0, &ch, 1) == 1)
	{
		//rt_kprintf("<=%x \r\n",ch);
		switch(get)
		{
			case 0:
				if(ch==0xaa)
				{
					//rt_kprintf("0xaa get ,get =1\r\n");
					get=1;
				}
				break;
			case 1:
				if(ch==0x73)
				{
				//rt_kprintf("0x73 get ,get =2\r\n");
					get=2;
					}
				else if(ch==0x72)
					{
					//rt_kprintf("0x72 get ,get =3\r\n");
					get=3;
					}
				break;
			case 2:
				{
					//rt_kprintf("==> %x\r\n",ch);
					g_rx_tp_buf[i++]=ch;
					if(i==5)
					{
						i=1;
						get=4;
						type=1;
					//	rt_kprintf("press receive over,get =4\r\n");
					}
					else
						get=2;
				break;
				}
			case 3:
				{
					g_rx_tp_buf[i++]=ch;
				//	rt_kprintf("<== %x\r\n",ch);
					if(i==5)
					{
						i=1;
						get=4;
						type=0;
				//		rt_kprintf("up receive over,get =4\r\n");
					}
					else
						get=3;
				break;
				}
			case 4:
				if(ch==0xcc)
					{
				//	rt_kprintf("0xcc get ,get =5\r\n");
					get=5;
					}
				break;
			case 5:
				if(ch==0x33)
					{
				//	rt_kprintf("0x33 get ,get =6\r\n");
					get=6;
					}
				break;
			case 6:
				if(ch==0xc3)
					{
				//	rt_kprintf("0xc3 get ,get =7\r\n");
					get=7;
					}
				break;
			case 7:
				if(ch==0x3c)
				{
					get=0;
					g_rx_tp_buf[0]=type;
					rt_kprintf("get %x %x %x %x %x\r\n",g_rx_tp_buf[0],g_rx_tp_buf[1],g_rx_tp_buf[2],g_rx_tp_buf[3],g_rx_tp_buf[4]);

				
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

void DrawPicFast_Real(unsigned char Index)	//??????????
{
	uart_tx(UART_TYPE_LCD,0xAA);
	uart_tx(UART_TYPE_LCD,0x70);//draw pic fast at (0,0)
	uart_tx(UART_TYPE_LCD,Index);
	uart_tx(UART_TYPE_LCD,0xCC);
	uart_tx(UART_TYPE_LCD,0x33);
	uart_tx(UART_TYPE_LCD,0xC3);
	uart_tx(UART_TYPE_LCD,0x3C);
}
void DrawPic_Real(unsigned short x,unsigned short y,unsigned char Index,unsigned short Pic_Xs,unsigned short Pic_Ys,unsigned short Pic_Xe,unsigned short Pic_Ye)
{																											  
	uart_tx(UART_TYPE_LCD,0xAA);
	uart_tx(UART_TYPE_LCD,0x71);//draw pic
	uart_tx(UART_TYPE_LCD,Index);
	
	uart_tx(UART_TYPE_LCD,(Pic_Xs>>8)&0xff);
	uart_tx(UART_TYPE_LCD,Pic_Xs&0xff);	
	uart_tx(UART_TYPE_LCD,(Pic_Ys>>8)&0xff);
	uart_tx(UART_TYPE_LCD,Pic_Ys&0xff);
	uart_tx(UART_TYPE_LCD,(Pic_Xe>>8)&0xff);
	uart_tx(UART_TYPE_LCD,Pic_Xe&0xff);
	uart_tx(UART_TYPE_LCD,(Pic_Ye>>8)&0xff);
	uart_tx(UART_TYPE_LCD,Pic_Ye&0xff);
	uart_tx(UART_TYPE_LCD,(x>>8)&0xff);
	uart_tx(UART_TYPE_LCD,x&0xff);
	uart_tx(UART_TYPE_LCD,(y>>8)&0xff);
	uart_tx(UART_TYPE_LCD,y&0xff);
	
	uart_tx(UART_TYPE_LCD,0xCC);
	uart_tx(UART_TYPE_LCD,0x33);
	uart_tx(UART_TYPE_LCD,0xC3);
	uart_tx(UART_TYPE_LCD,0x3C);
	
}
#define STATE_ORIGIN 0
#define STATE_PIC1 1
#define STATE_PIC2 2
#define STATE_BEGIN_TEST 3
#define STATE_M2 4
#define STATE_M3 5
#define STATE_INPUT 6
#define STATE_RESULT 7
#define STATE_SAFE_POWEROFF 8

unsigned char CheckKeyPressedArea(int type)
{
	unsigned char i;
	unsigned short x,y;
	x=(g_rx_tp_buf[1]<<8)|g_rx_tp_buf[2];
	y=(g_rx_tp_buf[3]<<8)|g_rx_tp_buf[4];
	if(type==0)
	{//input 
		for(i=0;i<15;i++)
		{
			if( (x>(SBtnGroup_XY[i][0][0]))&&(x<(SBtnGroup_XY[i][1][0]))&&(y>(SBtnGroup_XY[i][0][1]))&&(y<(SBtnGroup_XY[i][1][1])) )
			{
				//KeyValue=i;
				rt_kprintf("%d is pressed in input panl\r\n",i);
				return i;
			}	
		}
	}
	else
	{//pic2
		for(i=0;i<4;i++)
		{
			if( (x>(SBmp2_XY[i][0][0]))&&(x<(SBmp2_XY[i][1][0]))&&(y>(SBmp2_XY[i][0][1]))&&(y<(SBmp2_XY[i][1][1])) )
			{
				//KeyValue=i;
				rt_kprintf("%d is pressed in bmp 2\r\n",i);
				return i;
			}	
		}

	}
	return 100;			
}
unsigned short input_handle()
{
	unsigned short key=0;
	int i=0,area=16;
	while(1)
	{
		while(g_rx_tp_buf[0]!=1)
			rt_thread_delay(20);
		rt_thread_delay(20);
		g_rx_tp_buf[0]=2;
		area=CheckKeyPressedArea(0);
		if(area!=100)
		{
			DrawPic_Real(SBtnGroup_XY[area][0][0],SBtnGroup_XY[area][0][1],4,SBtnGroup_XY[area][0][0],SBtnGroup_XY[area][0][1],SBtnGroup_XY[area][1][0],SBtnGroup_XY[area][1][1]);
			rt_thread_sleep(10);
			DrawPic_Real(SBtnGroup_XY[area][0][0],SBtnGroup_XY[area][0][1],3,SBtnGroup_XY[area][0][0],SBtnGroup_XY[area][0][1],SBtnGroup_XY[area][1][0],SBtnGroup_XY[area][1][1]);
			rt_kprintf("draw input panel side effect\r\n");
		}

		if(area==14)// enter
		{
			rt_kprintf("ENTER pressed , return key %d\r\n",key);
			return key;
		}
		else if(area==4)
		{//back space
			if(i>0)
			{
				key=key/10;
				i=i-1;

				if(i==0)
				{
					DrawPic_Real(340,62,3,208,62,259,133);
					DrawPic_Real(380,62,3,208,62,259,133);
					DrawPic_Real(420,62,7,SBnum_B_XY[0][0][0],SBnum_B_XY[0][0][1],SBnum_B_XY[0][1][0],SBnum_B_XY[0][1][1]);
					rt_kprintf("BackSpace pressed , draw 0\r\n");
				}
				else if(i==1)
				{
					DrawPic_Real(340,62,3,208,62,259,133);
					DrawPic_Real(380,62,3,208,62,259,133);
					DrawPic_Real(420,62,7,SBnum_B_XY[key][0][0],SBnum_B_XY[key][0][1],SBnum_B_XY[key][1][0],SBnum_B_XY[key][1][1]);
					rt_kprintf("BackSpace pressed , draw %d\r\n",key);
				}
				else if(i==2)
				{
					DrawPic_Real(340,62,3,208,62,259,133);
					DrawPic_Real(380,62,7,SBnum_B_XY[key/10][0][0],SBnum_B_XY[key/10][0][1],SBnum_B_XY[key/10][1][0],SBnum_B_XY[key/10][1][1]);
					rt_kprintf("BackSpace pressed , draw %d\r\n",key/10);
					DrawPic_Real(420,62,7,SBnum_B_XY[key%10][0][0],SBnum_B_XY[key%10][0][1],SBnum_B_XY[key%10][1][0],SBnum_B_XY[key%10][1][1]);
					rt_kprintf("BackSpace pressed , draw %d\r\n",key%10);
				}
				else if(i==3)
				{
					DrawPic_Real(340,62,7,SBnum_B_XY[key/100][0][0],SBnum_B_XY[key/100][0][1],SBnum_B_XY[key/100][1][0],SBnum_B_XY[key/100][1][1]);
					rt_kprintf("BackSpace pressed , draw %d\r\n",key/100);
					DrawPic_Real(380,62,7,SBnum_B_XY[(key%100)/10][0][0],SBnum_B_XY[(key%100)/10][0][1],SBnum_B_XY[(key%100)/10][1][0],SBnum_B_XY[(key%100)/10][1][1]);
					rt_kprintf("BackSpace pressed , draw %d\r\n",key%100);
					DrawPic_Real(420,62,7,SBnum_B_XY[key%10][0][0],SBnum_B_XY[key%10][0][1],SBnum_B_XY[key%10][1][0],SBnum_B_XY[key%10][1][1]);
					rt_kprintf("BackSpace pressed , draw %d\r\n",key%10);
				}
			}
			continue;
		}
		else if(area==9)
		{//clear

			i=0;
			key=0;
			DrawPic_Real(340,62,3,208,62,259,133);
			DrawPic_Real(380,62,3,208,62,259,133);
			DrawPic_Real(420,62,7,SBnum_B_XY[0][0][0],SBnum_B_XY[0][0][1],SBnum_B_XY[0][1][0],SBnum_B_XY[0][1][1]);
			rt_kprintf("Clear pressed , draw 0\r\n");
			continue;
		}
	if(i<3)
	{
		i++;
		switch(area)
		{
			case 0://6
				key=key*10+6;
			break;
			case 1://7
				key=key*10+7;
			break;
			case 2://8
				key=key*10+8;
			break;
			case 3://9
				key=key*10+9;
			break;
			case 5:// 2
				key=key*10+2;
			break;
			case 6:// 3
				key=key*10+3;
			break;
			case 7:// 4
				key=key*10+4;
			break;
			case 8:// 5
				key=key*10+5;
			break;
			case 10://0
				key=key*10+0;
			break;
			case 11:// 1   
				key=key*10+1;
			break;
			
			default:
				if(i>0)
					i--;
				break;
		}	
		if(i==1)
		{
			DrawPic_Real(340,62,3,208,62,259,133);
			DrawPic_Real(380,62,3,208,62,259,133);
			DrawPic_Real(420,62,7,SBnum_B_XY[key][0][0],SBnum_B_XY[key][0][1],SBnum_B_XY[key][1][0],SBnum_B_XY[key][1][1]);
			rt_kprintf("o%d=>%d pressed , draw it\r\n",i,key);
		}
		else if(i==2)
		{
			DrawPic_Real(340,62,3,208,62,259,133);
			DrawPic_Real(380,62,7,SBnum_B_XY[key/10][0][0],SBnum_B_XY[key/10][0][1],SBnum_B_XY[key/10][1][0],SBnum_B_XY[key/10][1][1]);
			DrawPic_Real(420,62,7,SBnum_B_XY[(key%100)%10][0][0],SBnum_B_XY[(key%100)%10][0][1],SBnum_B_XY[(key%100)%10][1][0],SBnum_B_XY[(key%100)%10][1][1]);
			rt_kprintf("oo%d=>%d%d pressed , draw it\r\n",i,key/10,(key%100)%10);
		}
		else if(i==3)
		{
			DrawPic_Real(340,62,7,SBnum_B_XY[key/100][0][0],SBnum_B_XY[key/100][0][1],SBnum_B_XY[key/100][1][0],SBnum_B_XY[key/100][1][1]);
			DrawPic_Real(380,62,7,SBnum_B_XY[(key%100)/10][0][0],SBnum_B_XY[(key%100)/10][0][1],SBnum_B_XY[(key%100)/10][1][0],SBnum_B_XY[(key%100)/10][1][1]);
			DrawPic_Real(420,62,7,SBnum_B_XY[(key%100)%10][0][0],SBnum_B_XY[(key%100)%10][0][1],SBnum_B_XY[(key%100)%10][1][0],SBnum_B_XY[(key%100)%10][1][1]);
			rt_kprintf("ooo%d=>%d%d%d pressed , draw it\r\n",i,key/100,(key%100)/10,(key%100)%10);
		}
		
		}
		//g_rx_tp_buf[0]=2;
	}
}
void main_loop()
{
	int state=0,last_state=0,mianji_val_input=0,liushu_val_input=0;
	rt_err_t result;
	static struct rt_thread thread,thread1;
	DrawPicFast_Real(0);
	rt_thread_delay(1*100);
	/*DrawPicFast_Real(1);
	rt_thread_delay(2*100);
	DrawPicFast_Real(2);
	rt_thread_delay(2*100);
	DrawPicFast_Real(3);
	rt_thread_delay(2*100);
	DrawPicFast_Real(4);
	rt_thread_delay(2*100);
	DrawPicFast_Real(5);
	rt_thread_delay(2*100);
	DrawPicFast_Real(6);
	rt_thread_delay(2*100);
	DrawPicFast_Real(7);
	rt_thread_delay(2*100);
	*/
	state=STATE_PIC1;	
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
					rt_kprintf("main_loop 7\r\n");
					DrawPicFast_Real(0);
					state=STATE_PIC1;
					last_state=STATE_ORIGIN;
					rt_kprintf("STATE_ORIGIN state , goto STATE_PIC1\r\n");
					break;
				}
				case STATE_PIC1:
				{
					rt_kprintf("STATE_PIC1 , last state %d\r\n",last_state);	
					DrawPicFast_Real(1);
					//if(last_state==STATE_M2||last_state==STATE_M3||last_state==STATE_BEGIN_TEST)
					{
						//if(mianji_val_input!=0)
						{
							if((mianji_val_input/100)!=0)
							{
								DrawPic_Real(200,218,5,SBnum_L_XY[mianji_val_input/100][0][0],SBnum_L_XY[mianji_val_input/100][0][1],SBnum_L_XY[mianji_val_input/100][1][0],SBnum_L_XY[mianji_val_input/100][1][1]);
							}
							
							if(((mianji_val_input%100)/10)!=0||(mianji_val_input/100)!=0)
							{
								DrawPic_Real(221,218,5,SBnum_L_XY[(mianji_val_input%100)/10][0][0],SBnum_L_XY[(mianji_val_input%100)/10][0][1],SBnum_L_XY[(mianji_val_input%100)/10][1][0],SBnum_L_XY[(mianji_val_input%100)/10][1][1]);
							}
							
							DrawPic_Real(240,218,5,SBnum_L_XY[(mianji_val_input%100)%10][0][0],SBnum_L_XY[(mianji_val_input%100)%10][0][1],SBnum_L_XY[(mianji_val_input%100)%10][1][0],SBnum_L_XY[(mianji_val_input%100)%10][1][1]);
						}
						//if(liushu_val_input!=0)
						{
							if((liushu_val_input/100)!=0)
							{
								DrawPic_Real(470,218,5,SBnum_L_XY[liushu_val_input/100][0][0],SBnum_L_XY[liushu_val_input/100][0][1],SBnum_L_XY[liushu_val_input/100][1][0],SBnum_L_XY[liushu_val_input/100][1][1]);
							}
							
							if(((liushu_val_input%100)/10)!=0||(liushu_val_input/100)!=0)
							{
								DrawPic_Real(490,218,5,SBnum_L_XY[(liushu_val_input%100)/10][0][0],SBnum_L_XY[(liushu_val_input%100)/10][0][1],SBnum_L_XY[(liushu_val_input%100)/10][1][0],SBnum_L_XY[(liushu_val_input%100)/10][1][1]);
							}
							
							DrawPic_Real(511,218,5,SBnum_L_XY[liushu_val_input%10][0][0],SBnum_L_XY[liushu_val_input%10][0][1],SBnum_L_XY[liushu_val_input%10][1][0],SBnum_L_XY[liushu_val_input%10][1][1]);
						}
					}
					long val_serial_bak=val_serial;
					long val_serial_bak1=val_serial;
					if(last_state==STATE_BEGIN_TEST)
					{
						int i=0;
							/*if((val_serial_bak/100)!=0)
							{
								DrawPic_Real(422,124,6,SBnum_L_XY[val_serial_bak/100][0][0],SBnum_L_XY[val_serial_bak/100][0][1],SBnum_L_XY[val_serial_bak/100][1][0],SBnum_L_XY[val_serial_bak/100][1][1]);
							}
							
							if(((val_serial_bak%100)/10)!=0||(val_serial_bak/100)!=0)
							{
								DrawPic_Real(445,124,6,SBnum_L_XY[(val_serial_bak%100)/10][0][0],SBnum_L_XY[(val_serial_bak%100)/10][0][1],SBnum_L_XY[(val_serial_bak%100)/10][1][0],SBnum_L_XY[(val_serial_bak%100)/10][1][1]);
							}
							
							DrawPic_Real(468,124,6,SBnum_L_XY[val_serial_bak%10][0][0],SBnum_L_XY[val_serial_bak%10][0][1],SBnum_L_XY[val_serial_bak%10][1][0],SBnum_L_XY[val_serial_bak%10][1][1]);
							*/
						while(val_serial_bak1!=0)
						{
							
							DrawPic_Real(468-i*23,124,6,SBnum_L_XY[val_serial_bak1%10][0][0],SBnum_L_XY[val_serial_bak1%10][0][1],SBnum_L_XY[val_serial_bak1%10][1][0],SBnum_L_XY[val_serial_bak1%10][1][1]);
							i++;
							val_serial_bak1=val_serial_bak1/10;
						}

					
					if(val_serial_bak!=0&&liushu_val_input!=0&&mianji_val_input!=0)
					{
						int result=(int)(val_serial_bak*liushu_val_input*mianji_val_input);
						i=0;
						rt_kprintf("result is %d,val_serial_bak is %d , liusu is %d, mianji is %d\r\n",result,val_serial_bak,liushu_val_input,mianji_val_input);
						while(result!=0)
						{
							
							DrawPic_Real(474-i*32,313,6,SBnum_L_XY[result%10][0][0],SBnum_L_XY[result%10][0][1],SBnum_L_XY[result%10][1][0],SBnum_L_XY[result%10][1][1]);
							i++;
							result=result/10;
						}/*
						if((result/1000)!=0)
						{
							DrawPic_Real(414,313,6,SBnum_L_XY[result/1000][0][0],SBnum_L_XY[result/1000][0][1],SBnum_L_XY[result/1000][1][0],SBnum_L_XY[result/1000][1][1]);
						}
						
						if(((result%1000)/100)!=0||(result/1000)!=0)
						{
							DrawPic_Real(434,313,6,SBnum_L_XY[(result%1000)/100][0][0],SBnum_L_XY[(result%1000)/100][0][1],SBnum_L_XY[(result%1000)/100][1][0],SBnum_L_XY[(result%1000)/100][1][1]);
						}
						if(((result%100)/10)!=0||((result%1000)/100)!=0)
						{
							DrawPic_Real(454,313,6,SBnum_L_XY[(result%100)/10][0][0],SBnum_L_XY[(result%100)/10][0][1],SBnum_L_XY[(result%100)/10][1][0],SBnum_L_XY[(result%100)/10][1][1]);
						}

						DrawPic_Real(474,313,6,SBnum_L_XY[result%10][0][0],SBnum_L_XY[result%10][0][1],SBnum_L_XY[result%10][1][0],SBnum_L_XY[result%10][1][1]);
						*/
					}}
					while(g_rx_tp_buf[0]!=1)
						rt_thread_delay(20);
					g_rx_tp_buf[0]=2;
					switch(CheckKeyPressedArea(1))
					{
						case 0://jie mianji
						{
							state=STATE_M2;
							DrawPic_Real(SBmp2_XY[0][0][0],SBmp2_XY[0][0][1],2,SBmp2_XY[0][0][0],SBmp2_XY[0][0][1],SBmp2_XY[0][1][0],SBmp2_XY[0][1][1]);
							break;
						}
						case 1://pin jun liusu:
						{
							state=STATE_M3;
							DrawPic_Real(SBmp2_XY[1][0][0],SBmp2_XY[1][0][1],2,SBmp2_XY[1][0][0],SBmp2_XY[1][0][1],SBmp2_XY[1][1][0],SBmp2_XY[1][1][1]);
							break;
						}
						case 2:
						{
							state=STATE_BEGIN_TEST;
							DrawPic_Real(SBmp2_XY[2][0][0],SBmp2_XY[2][0][1],2,SBmp2_XY[2][0][0],SBmp2_XY[2][0][1],SBmp2_XY[2][1][0],SBmp2_XY[2][1][1]);
							break;
						}
						case 3:
						{
							state=STATE_SAFE_POWEROFF;
							DrawPic_Real(SBmp2_XY[3][0][0],SBmp2_XY[3][0][1],2,SBmp2_XY[3][0][0],SBmp2_XY[3][0][1],SBmp2_XY[3][1][0],SBmp2_XY[3][1][1]);
							break;
						}
					}	
					last_state=STATE_PIC1;
					//if(state==STATE_M2||state==STATE_M3||state==STATE_BEGIN_TEST||state==STATE_SAFE_POWEROFF)
					//	DrawPicFast_Real(2);
					rt_thread_delay(20);
					rt_kprintf("STATE_PIC1 state , goto %d\r\n",state);
					break;
				}
				case STATE_M2:
				{
					DrawPicFast_Real(3);
					mianji_val_input=input_handle();
					state=STATE_PIC1;
					last_state=STATE_M2;
					rt_kprintf("STATE_M2 state , goto STATE_PIC1, mianji_val_input %d\r\n",mianji_val_input);
					break;
				}
				case STATE_M3:
				{
					DrawPicFast_Real(3);
					liushu_val_input=input_handle();
					state=STATE_PIC1;
					last_state=STATE_M3;
					rt_kprintf("STATE_M3 state , goto STATE_PIC1, liushu_val_input %d\r\n",liushu_val_input);
					break;
				}				
				case STATE_BEGIN_TEST:
				{
					unsigned char ch='S';
					uart_tx(UART_TYPE_PARAM,ch);
					rt_thread_delay(100);
					state=STATE_PIC1;
					last_state=STATE_BEGIN_TEST;
					rt_kprintf("STATE_BEGIN_TEST state , goto STATE_PIC1, val_serial %d\r\n",val_serial);
					break;
				}
				case STATE_SAFE_POWEROFF:
				{
					DrawPicFast_Real(8);
					rt_kprintf("STATE_SAFE_POWEROFF state , loop forever\r\n");
					while(1);
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
