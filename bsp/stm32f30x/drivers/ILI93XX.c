#include "ILI93XX.h"
#include "8x16.h"
#define  MAX_X  240
#define  MAX_Y  320  

#define  CS_H	GPIOC->ODR 		|=  (1<<9)     		//CS -- PC9
#define  CS_L	GPIOC->ODR 		&= ~(1<<9)

#define  RS_H	GPIOC->ODR 		|=  (1<<8)			//RS -- PC8
#define  RS_L	GPIOC->ODR 		&= ~(1<<8)

#define	 WR_H	GPIOC->ODR 		|=  (1<<7)			//RW -- PC7 
#define	 WR_L	GPIOC->ODR 		&= ~(1<<7)

#define  RD_H	GPIOC->ODR 		|=  (1<<6)			//RD -- PC6
#define  RD_L	GPIOC->ODR 		&= ~(1<<6)

#define  RES_H	GPIOC->ODR 		|=  (1<<5)   		//RST-- PC5
#define  RES_L	GPIOC->ODR 		&= ~(1<<5)

#define  PortData				(GPIOB->ODR)  	

void LCD_GPIO_Config()
{
		GPIO_InitTypeDef GPIO_InitStruct;	
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB|RCC_AHBPeriph_GPIOC, ENABLE); 
		GPIO_InitStruct.GPIO_Pin 		= 	GPIO_Pin_All;			
		GPIO_InitStruct.GPIO_Mode 	= 	GPIO_Mode_OUT;								
		GPIO_InitStruct.GPIO_PuPd 	= 	GPIO_PuPd_UP;									
		GPIO_InitStruct.GPIO_Speed 	= 	GPIO_Speed_50MHz;							
		GPIO_Init(GPIOB, &GPIO_InitStruct); 													
		GPIO_InitStruct.GPIO_Pin 		= 	GPIO_Pin_6+GPIO_Pin_7+GPIO_Pin_8+GPIO_Pin_9;			
		GPIO_InitStruct.GPIO_Mode 	= 	GPIO_Mode_OUT;								
		GPIO_InitStruct.GPIO_PuPd 	= 	GPIO_PuPd_UP;							
		GPIO_InitStruct.GPIO_Speed 	= 	GPIO_Speed_50MHz;						
		GPIO_Init(GPIOC, &GPIO_InitStruct); 								
}

void delayms(uint16_t ms)    
{ 
	uint16_t i,j; 
	for( i = 0; i < ms; i++ )
	{ 
		for( j = 0; j < 1141; j++ );
	}
	//rt_thread_delay(ms);
} 

void LCD_Write_Cmd(uint16_t Cmd)
{  
	 CS_L;
   	 RS_L;
	 RD_H;
	 PortData = Cmd ;
	 WR_L;
	 WR_H;
	 CS_H;
}

void LCD_Write_Data(uint16_t Data)
{
	 CS_L;
	 RS_H;
	 RD_H;
	 PortData = Data ;
	 WR_L;
	 WR_H;
	 CS_H;
}
uint16_t LCD_Read_Data(void)
{
	 uint16_t value;
	 
	 CS_L;
	 RS_H;
	 WR_H;
	 RD_L;	
	 GPIOB->MODER = 0X00000000;
	 value = GPIOB->IDR ;	
	 GPIOB->MODER = 0X55555555;	
	 RD_H; 
	 CS_H;
	 return value;
}
uint16_t Read_Point(uint16_t x,uint16_t y)
{
	uint16_t val;
	Write_Cmd_Data(0x0020,x);
  	Write_Cmd_Data(0x0021,y);	
	LCD_Write_Cmd(0x0022);
	val = LCD_Read_Data();
	val = LCD_Read_Data();		
	return val;
}
uint16_t Read_ID(void)
{
	uint16_t val;
	LCD_Write_Cmd(0x0000);
	val = LCD_Read_Data();
	return val;
}	


void Write_Cmd_Data(uint16_t Cmd, uint16_t Data)
{
	LCD_Write_Cmd(Cmd);
	LCD_Write_Data(Data);	
}

void TFT_Init()
{
	LCD_GPIO_Config();                
	CS_L;
	RES_L;
	delayms(100);
	RES_H;
	delayms(100);
	int id=Read_ID();
	rt_kprintf("LCD Id %04x",id);
	Write_Cmd_Data(0x0001,0x0100);	  /* Driver Output Contral Register */ 
	Write_Cmd_Data(0x0002,0x0700);    /* LCD Driving Waveform Contral */
	Write_Cmd_Data(0x0003,0x1030);	  /* Entry Mode setting */
	
	Write_Cmd_Data(0x0004,0x0000);	  /* Scalling Control register */
	Write_Cmd_Data(0x0008,0x0207);	  /* Display Control 2 */
	Write_Cmd_Data(0x0009,0x0000);	  /* Display Control 3 */
	Write_Cmd_Data(0x000A,0x0000);	  /* Frame Cycle Control */
	Write_Cmd_Data(0x000C,0x0000);	  /* External Display Interface Control 1 */
	Write_Cmd_Data(0x000D,0x0000);    /* Frame Maker Position */
	Write_Cmd_Data(0x000F,0x0000);	  /* External Display Interface Control 2 */
	delayms(50);
	Write_Cmd_Data(0x0007,0x0101);	  /* Display Control */
	delayms(50);
	Write_Cmd_Data(0x0010,0x16B0);    /* Power Control 1 */
	Write_Cmd_Data(0x0011,0x0001);    /* Power Control 2 */
	Write_Cmd_Data(0x0017,0x0001);    /* Power Control 3 */
	Write_Cmd_Data(0x0012,0x0138);    /* Power Control 4 */
	Write_Cmd_Data(0x0013,0x0800);    /* Power Control 5 */
	Write_Cmd_Data(0x0029,0x0009);	  /* NVM read data 2 */
	Write_Cmd_Data(0x002a,0x0009);	  /* NVM read data 3 */
	Write_Cmd_Data(0x00a4,0x0000);
	Write_Cmd_Data(0x0050,0x0000);	  
	Write_Cmd_Data(0x0051,0x00EF);	  
	Write_Cmd_Data(0x0052,0x0000);	 
	Write_Cmd_Data(0x0053,0x013F);	  
	
	Write_Cmd_Data(0x0060,0xA700);	  /* Driver Output Control */  								  
	Write_Cmd_Data(0x0061,0x0003);	  /* Driver Output Control */
	Write_Cmd_Data(0x006A,0x0000);	  /* Vertical Scroll Control */
	
	Write_Cmd_Data(0x0080,0x0000);	  /* Display Position – Partial Display 1 */
	Write_Cmd_Data(0x0081,0x0000);	  /* RAM Address Start – Partial Display 1 */
	Write_Cmd_Data(0x0082,0x0000);	  /* RAM address End - Partial Display 1 */
	Write_Cmd_Data(0x0083,0x0000);	  /* Display Position – Partial Display 2 */
	Write_Cmd_Data(0x0084,0x0000);	  /* RAM Address Start – Partial Display 2 */
	Write_Cmd_Data(0x0085,0x0000);	  /* RAM address End – Partail Display2 */
	Write_Cmd_Data(0x0090,0x0013);	  /* Frame Cycle Control */
	Write_Cmd_Data(0x0092,0x0000); 	  /* Panel Interface Control 2 */
	Write_Cmd_Data(0x0093,0x0003);	  /* Panel Interface control 3 */
	Write_Cmd_Data(0x0095,0x0110);	  /* Frame Cycle Control */
	Write_Cmd_Data(0x0007,0x0173); 						
  	delayms(50);   /* delay 50 ms */
}

void TFT_ClearScreen(uint16 color)
{
	uint32_t index=0;
	
	//TFT_SetWindow(0,240,0,320);
	Write_Cmd_Data(0X0020,0);
	Write_Cmd_Data(0X0021,0);
	LCD_Write_Cmd(0x0022);
	for( index = 0; index < MAX_X * MAX_Y; index++ )
	{
		LCD_Write_Data(color);		
	}		
}
void  TFT_SetWindow(uint16_t xStart, uint16_t xEnd, uint16_t yStart, uint16_t yEnd)
{
	Write_Cmd_Data(0X0050,xStart);	  
	Write_Cmd_Data(0X0051,xEnd);	  
	Write_Cmd_Data(0X0052,yStart);	  
	Write_Cmd_Data(0X0053,yEnd);	  
	Write_Cmd_Data(0X0020,xStart);	 
	Write_Cmd_Data(0X0021,yStart);	  
	LCD_Write_Cmd(0x0022);				  
}
void LCD_SetPoint(uint16_t Xpos,uint16_t Ypos,uint16_t point)
{
	TFT_SetWindow(Xpos,Xpos+1,Ypos,Ypos+1);
	Write_Cmd_Data(0x0022,point);
}

void  GUI_Point(uint16 x, uint16 y, uint16 color)
{	    
	LCD_SetPoint(x,		y,	color);
	LCD_SetPoint(x+1,	y,	color);
	LCD_SetPoint(x,		y+1,color);
	LCD_SetPoint(x+1, 	y+1,color); 	  	
}		

void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; 
	else if(delta_x==0)incx=0;
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )
	{  
		LCD_SetPoint(uRow,uCol,0XF800);
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}

void LCD_PutChar(unsigned short x, unsigned short y, char c, unsigned int fColor, unsigned int bColor)
{
 unsigned int i,j;
 TFT_SetWindow(x,x+8-1,y,y+16-1);
 for(i=0; i<16;i++) {
		unsigned char m=Font8x16[c*16+i];
		for(j=0;j<8;j++) {
			if((m&0x80)==0x80) {
				LCD_Write_Data(fColor);
				}
			else {
				LCD_Write_Data(bColor);
				}
			m<<=1;
			}
		}
}
void GUI_DispColor(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend, unsigned int color)
{
    uint16  width, height;
    uint16  i,j;

    TFT_SetWindow(Xstart, Xend, Ystart, Yend);

    width  = Xend - Xstart;
    height = Yend - Ystart;

    for(i = 0; i <= width; i++)
    for(j = 0; j <= height; j++)
    {    
//        TFT_WriteData(color);
        Write_Cmd_Data(0x0022,color);
    }
}
void TFT_WriteData(uint16 dat)
{
	LCD_Write_Data(dat);
}
