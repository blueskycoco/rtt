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
	285,227,308,254,// 0
	308,227,331,254,// 1
	331,227,354,254,// 2
	354,227,377,254,// 3
	377,227,400,254,// 4
	400,227,423,254,// 5
	423,227,446,254,// 6
	446,227,469,254,// 7
	469,227,492,254,// 8
	492,227,515,254// 9
};
unsigned short const SBnum_B_XY[10][2][2]=
{//index=8
	130,209,181,271,// 0
	181,209,232,271,// 1
	232,209,283,271,// 2
	283,209,334,271,// 3
	334,209,385,271,// 4
	385,209,436,271,// 5
	436,209,487,271,// 6
	487,209,540,271,// 7
	540,209,591,271,// 8
	591,209,642,271// 9
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
unsigned short input_handle()
{
	unsigned short key=0;
	int i,area=16;
	while(1)
	{
		while(g_rx_tp_buf[0]!=1)
			rt_thread_delay(20);
		area=CheckKeyPressedArea(0)
		if(area!=100)
		{
			DrawPic_Real(SBtnGroup_XY[area][0][0],SBtnGroup_XY[area][0][1],4,SBtnGroup_XY[area][0][0],SBtnGroup_XY[area][0][1],SBtnGroup_XY[area][1][0],SBtnGroup_XY[area][1][1]);
			rt_thread_sleep(100);
			DrawPic_Real(SBtnGroup_XY[area][0][0],SBtnGroup_XY[area][0][1],3,SBtnGroup_XY[area][0][0],SBtnGroup_XY[area][0][1],SBtnGroup_XY[area][1][0],SBtnGroup_XY[area][1][1]);

		}

		if(area==14)// enter
		{
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
					DrawPic_Real(442,52,3,208,52,259,133);
					DrawPic_Real(493,52,3,208,52,259,133);
					DrawPic_Real(544,52,7,SBnum_B_XY[0][0][0],SBnum_B_XY[0][0][1],SBnum_B_XY[0][1][0],SBnum_B_XY[0][1][1]);
				}
				else if(i==1)
				{
					DrawPic_Real(442,52,3,208,52,259,133);
					DrawPic_Real(493,52,3,208,52,259,133);
					DrawPic_Real(544,52,7,SBnum_B_XY[key][0][0],SBnum_B_XY[key][0][1],SBnum_B_XY[key][1][0],SBnum_B_XY[key][1][1]);

				}
				else if(i==2)
				{
					DrawPic_Real(442,52,3,208,52,259,133);
					DrawPic_Real(493,52,7,SBnum_B_XY[key/10][0][0],SBnum_B_XY[key/10][0][1],SBnum_B_XY[key/10][1][0],SBnum_B_XY[key/10][1][1]);
					DrawPic_Real(544,52,7,SBnum_B_XY[key%10][0][0],SBnum_B_XY[key%10][0][1],SBnum_B_XY[key%10][1][0],SBnum_B_XY[key%10][1][1]);

				}
				else if(i==3)
				{
					DrawPic_Real(442,52,7,SBnum_B_XY[key/100][0][0],SBnum_B_XY[key/100][0][1],SBnum_B_XY[key/100][1][0],SBnum_B_XY[key/100][1][1]);
					DrawPic_Real(493,52,7,SBnum_B_XY[(key%100)/10][0][0],SBnum_B_XY[(key%100)/10][0][1],SBnum_B_XY[(key%100)/10][1][0],SBnum_B_XY[(key%100)/10][1][1]);
					DrawPic_Real(544,52,7,SBnum_B_XY[(key%100)%10][0][0],SBnum_B_XY[(key%100)%10][0][1],SBnum_B_XY[(key%100)%10][1][0],SBnum_B_XY[(key%100)%10][1][1]);

				}
			}
		}
		else if(area==9)
		{//clear

			i=0;
			key=0;

			DrawPic_Real(442,52,3,208,52,259,133);
			DrawPic_Real(493,52,3,208,52,259,133);
			DrawPic_Real(544,52,7,SBnum_B_XY[0][0][0],SBnum_B_XY[0][0][1],SBnum_B_XY[0][1][0],SBnum_B_XY[0][1][1]);

		}
	if(i<=3)
	{
		i++;
		switch(area)
		{
			case 0://6
				key=key*10+6;
			break;
			case 1//7
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
			DrawPic_Real(442,52,3,208,52,259,133);
			DrawPic_Real(493,52,3,208,52,259,133);
			DrawPic_Real(544,52,7,SBnum_B_XY[key][0][0],SBnum_B_XY[key][0][1],SBnum_B_XY[key][1][0],SBnum_B_XY[key][1][1]);
		}
		else if(i==2)
		{
			DrawPic_Real(442,52,3,208,52,259,133);
			DrawPic_Real(493,52,7,SBnum_B_XY[key/10][0][0],SBnum_B_XY[key/10][0][1],SBnum_B_XY[key/10][1][0],SBnum_B_XY[key/10][1][1]);
			DrawPic_Real(544,52,7,SBnum_B_XY[key%10][0][0],SBnum_B_XY[key%10][0][1],SBnum_B_XY[key%10][1][0],SBnum_B_XY[key%10][1][1]);
		}
		else if(i==3)
		{
			DrawPic_Real(442,52,7,SBnum_B_XY[key/100][0][0],SBnum_B_XY[key/100][0][1],SBnum_B_XY[key/100][1][0],SBnum_B_XY[key/100][1][1]);
			DrawPic_Real(493,52,7,SBnum_B_XY[(key%100)/10][0][0],SBnum_B_XY[(key%100)/10][0][1],SBnum_B_XY[(key%100)/10][1][0],SBnum_B_XY[(key%100)/10][1][1]);
			DrawPic_Real(544,52,7,SBnum_B_XY[(key%100)%10][0][0],SBnum_B_XY[(key%100)%10][0][1],SBnum_B_XY[(key%100)%10][1][0],SBnum_B_XY[(key%100)%10][1][1]);
		}
		
		}
		g_rx_tp_buf[0]=2;
	}
}
void main_loop()
{
	int state=0,last_state=0,mianji_val_input=0,liushu_val_input=0,val_serial=0;
	DrawPicFast_Real(0);
	rt_thread_delay(2*100);
	state=STATE_PIC1;	
	last_state=STATE_ORIGIN;
	while(1)
	{
		switch(state)
			{
				case STATE_ORIGIN:
				{
					DrawPicFast_Real(0);
					state=STATE_PIC1;
					last_state=STATE_ORIGIN;
					break;
				}
				case STATE_PIC1:
				{
					DrawPicFast_Real(1);
					if(last_state==STATE_M2||last_state==STATE_M3)
					{
						//if(mianji_val_input!=0)
						{
							if((mianji_val_input/100)!=0)
							{
								DrawPic_Real(218,222,5,SBnum_B_XY[mianji_val_input/100][0][0],SBnum_B_XY[mianji_val_input/100][0][1],SBnum_B_XY[mianji_val_input/100][1][0],SBnum_B_XY[mianji_val_input/100][1][1]);
							}
							
							if(((mianji_val_input%100)/10)!=0||(mianji_val_input/100)!=0)
							{
								DrawPic_Real(241,222,5,SBnum_B_XY[(mianji_val_input%100)/10][0][0],SBnum_B_XY[(mianji_val_input%100)/10][0][1],SBnum_B_XY[(mianji_val_input%100)/10][1][0],SBnum_B_XY[(mianji_val_input%100)/10][1][1]);
							}
							
							DrawPic_Real(264,222,5,SBnum_B_XY[(mianji_val_input%100)%10][0][0],SBnum_B_XY[(mianji_val_input%100)%10][0][1],SBnum_B_XY[(mianji_val_input%100)%10][1][0],SBnum_B_XY[(mianji_val_input%100)%10][1][1]);
						}
						//if(liushu_val_input!=0)
						{
							if((liushu_val_input/100)!=0)
							{
								DrawPic_Real(575,222,5,SBnum_B_XY[liushu_val_input/100][0][0],SBnum_B_XY[liushu_val_input/100][0][1],SBnum_B_XY[liushu_val_input/100][1][0],SBnum_B_XY[liushu_val_input/100][1][1]);
							}
							
							if(((liushu_val_input%100)/10)!=0||(liushu_val_input/100)!=0)
							{
								DrawPic_Real(598,222,5,SBnum_B_XY[(liushu_val_input%100)/10][0][0],SBnum_B_XY[(liushu_val_input%100)/10][0][1],SBnum_B_XY[(liushu_val_input%100)/10][1][0],SBnum_B_XY[(liushu_val_input%100)/10][1][1]);
							}
							
							DrawPic_Real(621,222,5,SBnum_B_XY[liushu_val_input%10][0][0],SBnum_B_XY[liushu_val_input%10][0][1],SBnum_B_XY[liushu_val_input%10][1][0],SBnum_B_XY[liushu_val_input%10][1][1]);
						}
					}
					else if(last_state==STATE_BEGIN_TEST)
					{
							if((val_serial/100)!=0)
							{
								DrawPic_Real(422,122,6,SBnum_B_XY[val_serial/100][0][0],SBnum_B_XY[val_serial/100][0][1],SBnum_B_XY[val_serial/100][1][0],SBnum_B_XY[val_serial/100][1][1]);
							}
							
							if(((val_serial%100)/10)!=0||(val_serial/100)!=0)
							{
								DrawPic_Real(445,122,6,SBnum_B_XY[(val_serial%100)/10][0][0],SBnum_B_XY[(val_serial%100)/10][0][1],SBnum_B_XY[(val_serial%100)/10][1][0],SBnum_B_XY[(val_serial%100)/10][1][1]);
							}
							
							DrawPic_Real(468,122,6,SBnum_B_XY[val_serial%10][0][0],SBnum_B_XY[val_serial%10][0][1],SBnum_B_XY[val_serial%10][1][0],SBnum_B_XY[val_serial%10][1][1]);

					}
					if(val_serial!=0&&liushu_val_input!=0&&mianji_val_input!=0)
					{
						long result=val_serial*liushu_val_input*mianji_val_input;
						if((val_serial/1000)!=0)
						{
							DrawPic_Real(422,313,6,SBnum_B_XY[val_serial/1000][0][0],SBnum_B_XY[val_serial/1000][0][1],SBnum_B_XY[val_serial/1000][1][0],SBnum_B_XY[val_serial/1000][1][1]);
						}
						
						if(((val_serial%1000)/100)!=0||(val_serial/1000)!=0)
						{
							DrawPic_Real(445,313,6,SBnum_B_XY[(val_serial%1000)/100][0][0],SBnum_B_XY[(val_serial%1000)/100][0][1],SBnum_B_XY[(val_serial%1000)/100][1][0],SBnum_B_XY[(val_serial%1000)/100][1][1]);
						}
						if(((val_serial%100)/10)!=0||((val_serial%1000)/100)!=0)
						{
							DrawPic_Real(468,313,6,SBnum_B_XY[(val_serial%100)/10][0][0],SBnum_B_XY[(val_serial%100)/10][0][1],SBnum_B_XY[(val_serial%100)/10][1][0],SBnum_B_XY[(val_serial%100)/10][1][1]);
						}

						DrawPic_Real(491,313,6,SBnum_B_XY[val_serial%10][0][0],SBnum_B_XY[val_serial%10][0][1],SBnum_B_XY[val_serial%10][1][0],SBnum_B_XY[val_serial%10][1][1]);

					}
					while(g_rx_tp_buf[0]!=1)
						rt_thread_delay(20);
					switch(CheckKeyPressedArea(1))
					{
						case 0://jie mianji
						state=STATE_M2;
						break;
						case 1//pin jun liusu:
						state=STATE_M3;
						break;
						case 2:
						state=STATE_BEGIN_TEST;
						break;
						case 3:
						state=STATE_SAFE_POWEROFF;
						break;
					}	
					last_state=STATE_PIC1;
					break;
				}
				case STATE_M2:
				{
					DrawPicFast_Real(3);
					mianji_val_input=input_handle();
					state=STATE_PIC1;
					last_state=STATE_M2;
					break;
				}
				case STATE_M3:
				{
					DrawPicFast_Real(3);
					liushu_val_input=input_handle();
					state=STATE_PIC1;
					last_state=STATE_M3;
					break;
				}
			}
		
	}
}
