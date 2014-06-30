#include <rtthread.h>
#include "stm32f10x.h"
#define UART_TYPE_LCD 0

char g_rx_tp_buf[5];//first byte is pressd 1 or up 0 or invaled 2
unsigned short const SBtnGroup_XY[15][2][2]=
{
	118,176,210,244,//6
 	238,176,328,244,//7
 	354,176,444,244,//8
 	468,176,562,244,//9
 	588,176,678,244,//back space
 	118,278,210,348,// 2
 	238,278,328,348,// 3
 	354,278,444,348,// 4
 	468,278,562,348,// 5
 	588,278,678,348,//cleaer
 	118,380,210,450,//0
 	238,380,328,450,// 1
 	354,380,444,450,//+
 	468,380,562,450,//-
 	588,380,678,450//enter
};
unsigned short const SBmp2_XY[4][2][2]=
{//index=2
	69,209,379,253,//jie mianji
	420,209,731,253,//pinjun liusu
	69,389,379,433,// begin test
	421,389,730,433//safe power off
};
unsigned short const SBnum_L_XY[10][2][2]=
{//index=6or7
	287,227,303,254,// 0
	310,227,328,254,// 1
	331,227,350,254,// 2
	353,227,375,254,// 3
	378,227,398,254,// 4
	400,227,421,254,// 5
	422,227,445,254,// 6
	446,227,469,254,// 7
	470,227,492,254,// 8
	493,227,514,254// 9
};
unsigned short const SBnum_B_XY[10][2][2]=
{//index=8
	130,209,172,271,// 0
	189,209,227,271,// 1
	232,209,277,271,// 2
	284,209,331,271,// 3
	334,209,382,271,// 4
	387,209,434,271,// 5
	438,209,489,271,// 6
	493,209,543,271,// 7
	545,209,593,271,// 8
	595,209,645,271// 9
};
static rt_err_t uart_lcd_rx_ind(rt_device_t dev, rt_size_t size)
{
	char ch;
	int i=0;
	int get=2;//0 for up , 1 press , 2 for invaled
	while (rt_device_read(dev, 0, &ch, 1) == 1)
	{
		if(ch==0xaa)
		{
			if(rt_device_read(dev,0,&ch,1)==1)
			{
				if(ch==0x73)
				{
					get=1;
					if(rt_device_read(dev,0,&ch,1)==1)
					{
						g_rx_tp_buf[1]=ch;
					}
					else
						get=2;
					if((rt_device_read(dev,0,&ch,1)==1) && (get==1))
					{
						g_rx_tp_buf[2]=ch;
					}
					else
						get=2;
					if((rt_device_read(dev,0,&ch,1)==1) && (get==1))
					{
						g_rx_tp_buf[3]=ch;
					}
					else
						get=2;
					if((rt_device_read(dev,0,&ch,1)==1) && (get==1))
					{
						g_rx_tp_buf[4]=ch;
					}
					else
						get=2;

				}
				else if(ch==0x72)
				{
					get=0;
					g_rx_tp_buf[1]=0;
					g_rx_tp_buf[2]=0;
					g_rx_tp_buf[3]=0;
					g_rx_tp_buf[4]=0;
					rt_device_read(dev,0,&ch,1);
					rt_device_read(dev,0,&ch,1);
					rt_device_read(dev,0,&ch,1);
					rt_device_read(dev,0,&ch,1);
				}
				else
				{
					g_rx_tp_buf[0]=2;
					continue;
				}
				g_rx_tp_buf[0]=2;
				if(rt_device_read(dev,0,&ch,1)==1)
				{
					if(ch==0xcc)
					{
						if(rt_device_read(dev,0,&ch,1)==1)
						{
							if(ch==0x33)
							{
								if(rt_device_read(dev,0,&ch,1)==1)
								{
									if(ch==0xc3)
									{
										if(rt_device_read(dev,0,&ch,1)==1)
										{
											if(ch==0x3c)
											{
												g_rx_tp_buf[0]=get;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
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
				return i;
			}	
		}

		}
	return 100;			
}

void main_loop()
{
	int state=0;
	DrawPicFast_Real(0);
	rt_thread_delay(2*100);
	state=STATE_PIC1;
	while(1)
	{
		switch(state)
			{
				case STATE_ORIGIN:
				{
					DrawPicFast_Real(0);
					state=STATE_PIC1;
					break;
				}
				case STATE_PIC1:
				{
					DrawPicFast_Real(1);
					while(g_rx_tp_buf[0]!=1)
						rt_thread_delay(20);
					switch(CheckKeyPressedArea(1))
						{
							case 0://jie mianji
							state=STATE_INPUT;
							break;
							case 1//pin jun liusu:
							state=STATE_INPUT;
							break;
							case 2:
							state=STATE_BEGIN_TEST;
							break;
							case 3:
							state=STATE_SAFE_POWEROFF;
							break;
						}
					

				}
			}
		
	}
}
