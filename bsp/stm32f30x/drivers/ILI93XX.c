

#include "ILI93XX.h"
#include "8x16.h"
#define  MAX_X  240
#define  MAX_Y  320  

/**********TFT液晶接口宏定义****************************/
#define  CS_H	GPIOC->ODR 		|=  (1<<9)     	//CS -- PC9
#define  CS_L	GPIOC->ODR 		&= ~(1<<9)

#define  RS_H	GPIOC->ODR 		|=  (1<<8)			//RS -- PC8
#define  RS_L	GPIOC->ODR 		&= ~(1<<8)

#define	 WR_H	GPIOC->ODR 		|=  (1<<7)			//RW -- PC7 
#define	 WR_L	GPIOC->ODR 		&= ~(1<<7)

#define  RD_H	GPIOC->ODR 		|=  (1<<6)			//RD -- PC6
#define  RD_L	GPIOC->ODR 		&= ~(1<<6)

#define  RES_H	GPIOC->ODR 	|=  (1<<5)   		//RST-- PC5
#define  RES_L	GPIOC->ODR 	&= ~(1<<5)

#define  PortData			(GPIOB->ODR)  				//输出数据端口 PB

/*=========================================
*-函数功能	: 初始液晶对应的IO口
*-输入参数	: 无
=========================================*/
void LCD_GPIO_Config()
{
		GPIO_InitTypeDef GPIO_InitStruct;	
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB|RCC_AHBPeriph_GPIOC, ENABLE); //使能PORTB,PORTC时钟
		/*******数据端口-->PB*****/			
		GPIO_InitStruct.GPIO_Pin 		= 	GPIO_Pin_All;			
		GPIO_InitStruct.GPIO_Mode 	= 	GPIO_Mode_OUT;								//输出
		GPIO_InitStruct.GPIO_PuPd 	= 	GPIO_PuPd_UP;									//上拉
		GPIO_InitStruct.GPIO_Speed 	= 	GPIO_Speed_50MHz;							//高速
		GPIO_Init(GPIOB, &GPIO_InitStruct); 													//初始PORTB
		/********控制信号引脚*****/
		GPIO_InitStruct.GPIO_Pin 		= 	GPIO_Pin_6+GPIO_Pin_7+GPIO_Pin_8+GPIO_Pin_9;			
		GPIO_InitStruct.GPIO_Mode 	= 	GPIO_Mode_OUT;								//输出
		GPIO_InitStruct.GPIO_PuPd 	= 	GPIO_PuPd_UP;									//上拉
		GPIO_InitStruct.GPIO_Speed 	= 	GPIO_Speed_50MHz;							//高速
		GPIO_Init(GPIOC, &GPIO_InitStruct); 													//初始PORTC
}
/*=========================================
*-函数功能	: 粗略的延迟
*-输入参数	: ms
=========================================*/
void delayms(uint16_t ms)    
{ 
	uint16_t i,j; 
	for( i = 0; i < ms; i++ )
	{ 
		for( j = 0; j < 1141; j++ );
	}
	//rt_thread_delay(ms);
} 

/*=========================================
*-函数功能	: 写指令
*-输入参数	: Cmd
=========================================*/
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
/*=========================================
*-函数功能	: 写数据
*-输入参数	: Data
=========================================*/
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
/*=========================================
*-函数功能	: 读数据
*-输入参数	: 无
=========================================*/
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
/*=========================================
*-函数功能	: 读某一点
*-输入参数	: 无
=========================================*/
uint16_t Read_Point(uint16_t x,uint16_t y)
{
	uint16_t val;
	Write_Cmd_Data(0x0020,x);
  	Write_Cmd_Data(0x0021,y);	
	LCD_Write_Cmd(0x0022);
	val = LCD_Read_Data();
	val = LCD_Read_Data();		//读两次
	return val;
}
/*=========================================
*-函数功能	: 读去液晶驱动IC
*-输入参数	: 无
=========================================*/
uint16_t Read_ID(void)
{
	uint16_t val;
	LCD_Write_Cmd(0x0000);
	val = LCD_Read_Data();
	return val;
}	


/*=========================================
*-函数功能	: 写指令,写数据
*-输入参数	: Cmd,Data
=========================================*/
void Write_Cmd_Data(uint16_t Cmd, uint16_t Data)
{
	LCD_Write_Cmd(Cmd);
	LCD_Write_Data(Data);	
}

/*=========================================
*-函数功能	: ILI93XX液晶初始化
*-输入参数	: 无
=========================================*/
void TFT_Init()
{
	LCD_GPIO_Config();                /*初始化IO口*/
	CS_L;
	RES_L;
	delayms(100);
	RES_H;
	delayms(100);
	int id=Read_ID();
	rt_kprintf("LCD Id %04x",id);
	Write_Cmd_Data(0x0001,0x0100);	  /* Driver Output Contral Register */ 
	Write_Cmd_Data(0x0002,0x0700);      /* LCD Driving Waveform Contral */
	Write_Cmd_Data(0x0003,0x1030);	  /* Entry Mode设置 */
	
	Write_Cmd_Data(0x0004,0x0000);	  /* Scalling Control register */
	Write_Cmd_Data(0x0008,0x0207);	  /* Display Control 2 */
	Write_Cmd_Data(0x0009,0x0000);	  /* Display Control 3 */
	Write_Cmd_Data(0x000A,0x0000);	  /* Frame Cycle Control */
	Write_Cmd_Data(0x000C,0x0000);	  /* External Display Interface Control 1 */
	Write_Cmd_Data(0x000D,0x0000);      /* Frame Maker Position */
	Write_Cmd_Data(0x000F,0x0000);	  /* External Display Interface Control 2 */
	delayms(50);
	Write_Cmd_Data(0x0007,0x0101);	  /* Display Control */
	delayms(50);
	Write_Cmd_Data(0x0010,0x16B0);      /* Power Control 1 */
	Write_Cmd_Data(0x0011,0x0001);      /* Power Control 2 */
	Write_Cmd_Data(0x0017,0x0001);      /* Power Control 3 */
	Write_Cmd_Data(0x0012,0x0138);      /* Power Control 4 */
	Write_Cmd_Data(0x0013,0x0800);      /* Power Control 5 */
	Write_Cmd_Data(0x0029,0x0009);	  /* NVM read data 2 */
	Write_Cmd_Data(0x002a,0x0009);	  /* NVM read data 3 */
	Write_Cmd_Data(0x00a4,0x0000);
//设置扫描方式  
	Write_Cmd_Data(0x0050,0x0000);	  /* 设置操作窗口的X轴开始列 */
	Write_Cmd_Data(0x0051,0x00EF);	  /* 设置操作窗口的X轴结束列 */
	Write_Cmd_Data(0x0052,0x0000);	  /* 设置操作窗口的Y轴开始行 */
	Write_Cmd_Data(0x0053,0x013F);	  /* 设置操作窗口的Y轴结束行 */
	
/* 设置屏幕的点数以及扫描的起始行 */	   
	Write_Cmd_Data(0x0060,0xA700);	  /* Driver Output Control 需要竖立屏幕改为2700 */  								  
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


/*=========================================
*-函数功能	: 清屏
*-输入参数	: Color
=========================================*/
void LCD_Clear(uint16_t Color)
{
	uint32_t index=0;
	
	SetWindow(0,240,0,320);
	for( index = 0; index < MAX_X * MAX_Y; index++ )
	{
		LCD_Write_Data(Color);		
	}		
}
/*=========================================
*-函数功能	: 设计坐标
*-输入参数	: Xpos，Ypos
=========================================*/
void SetWindow(unsigned int X_Start,unsigned int X_End,unsigned int Y_Start,unsigned int Y_End)
{
	Write_Cmd_Data(0X0050,X_Start);	  
	Write_Cmd_Data(0X0051,X_End);	  
	Write_Cmd_Data(0X0052,Y_Start);	  
	Write_Cmd_Data(0X0053,Y_End);	  
	Write_Cmd_Data(0X0020,X_Start);	 
	Write_Cmd_Data(0X0021,Y_Start);	  
	LCD_Write_Cmd(0x0022);				  
}
/*=========================================
*-函数功能	: 打点函数
*-输入参数	: Xpos，Ypos，point
=========================================*/
void LCD_SetPoint(uint16_t Xpos,uint16_t Ypos,uint16_t point)
{
	SetWindow(Xpos,Xpos+1,Ypos,Ypos+1);
	Write_Cmd_Data(0x0022,point);
}

/*=========================================
*-函数功能	: 画点触摸用到
*-输入参数	: X，Y，color
=========================================*/
void Draw_Pixel(uint16_t X,uint16_t Y,uint16_t color)
{	    
	LCD_SetPoint(X,		Y,	color);
	LCD_SetPoint(X+1,	Y,	color);
	LCD_SetPoint(X,		Y+1,color);
	LCD_SetPoint(X+1, 	Y+1,color); 	  	
}		

/*=========================================
*-函数功能	: 划线函数
*-输入参数	: x1,y1,x2,y2
=========================================*/
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
//R-5 G-6 B-5
u16 RGB(u8 R,u8 G,u8 B)
{
	return((u16)(R&0XF8)<<8|(u16)(G&0XFC)<<3|(u16)(B&0XF8)>>3);
} 

void LCD_PutChar(unsigned short x, unsigned short y, char c, unsigned int fColor, unsigned int bColor)
{
 unsigned int i,j;
 SetWindow(x,x+8-1,y,y+16-1);
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
#if 0
/*  设置像素点 颜色,X,Y */
void rt_hw_lcd_set_pixel(const char* pixel, int x, int y)
{
	LCD_SetPoint(x,		y,	*(rt_uint16_t*)pixel);
	LCD_SetPoint(x+1,	y,	*(rt_uint16_t*)pixel);
	LCD_SetPoint(x,		y+1,*(rt_uint16_t*)pixel);
	LCD_SetPoint(x+1, 	y+1,*(rt_uint16_t*)pixel); 	  	
}

/* 获取像素点颜色 */
void rt_hw_lcd_get_pixel(char* pixel, int x, int y)
{
    *(rt_uint16_t*)pixel = Read_Point(x, y);//?BGR2RGB( lcd_read_gram(x,y) );
}

/* 画水平线 */
void rt_hw_lcd_draw_hline(const char* pixel, int x1, int x2, int y)
{
    /* [5:4]-ID~ID0 [3]-AM-1垂直-0水平 */
    write_reg(0x0003,(1<<12)|(1<<5)|(1<<4) | (0<<3) );

    lcd_SetCursor(x1, y);
    rw_data_prepare(); /* Prepare to write GRAM */
    while (x1 < x2)
    {
        write_data( *(rt_uint16_t*)pixel );
        x1++;
    }
}

/* 垂直线 */
void rt_hw_lcd_draw_vline(const char* pixel, int x, int y1, int y2)
{
    /* [5:4]-ID~ID0 [3]-AM-1垂直-0水平 */
    write_reg(0x0003,(1<<12)|(1<<5)|(0<<4) | (1<<3) );

    lcd_SetCursor(x, y1);
    rw_data_prepare(); /* Prepare to write GRAM */
    while (y1 < y2)
    {
        write_data( *(rt_uint16_t*)pixel );
        y1++;
    }
}

/* ?? */
void rt_hw_lcd_draw_blit_line(const char* pixels, int x, int y, rt_size_t size)
{
    rt_uint16_t *ptr;

    ptr = (rt_uint16_t*)pixels;

    /* [5:4]-ID~ID0 [3]-AM-1垂直-0水平 */
    write_reg(0x0003,(1<<12)|(1<<5)|(1<<4) | (0<<3) );

    lcd_SetCursor(x, y);
    rw_data_prepare(); /* Prepare to write GRAM */
    while (size)
    {
        write_data(*ptr ++);
        size --;
    }
}

struct rt_device_graphic_ops lcd_ili_ops =
{
    rt_hw_lcd_set_pixel,
    rt_hw_lcd_get_pixel,
    rt_hw_lcd_draw_hline,
    rt_hw_lcd_draw_vline,
    rt_hw_lcd_draw_blit_line
};

struct rt_device _lcd_device;
static rt_err_t lcd_init(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t lcd_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_err_t lcd_close(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t lcd_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    switch (cmd)
    {
    case RTGRAPHIC_CTRL_GET_INFO:
    {
        struct rt_device_graphic_info *info;

        info = (struct rt_device_graphic_info*) args;
        RT_ASSERT(info != RT_NULL);

        info->bits_per_pixel = 16;
        info->pixel_format = RTGRAPHIC_PIXEL_FORMAT_RGB565P;
        info->framebuffer = RT_NULL;
        info->width = 240;
        info->height = 320;
    }
    break;

    case RTGRAPHIC_CTRL_RECT_UPDATE:
        /* nothong to be done */
        break;

    default:
        break;
    }

    return RT_EOK;
}

void rt_hw_lcd_init(void)
{
    /* register lcd device */
    _lcd_device.type  = RT_Device_Class_Graphic;
    _lcd_device.init  = lcd_init;
    _lcd_device.open  = lcd_open;
    _lcd_device.close = lcd_close;
    _lcd_device.control = lcd_control;
    _lcd_device.read  = RT_NULL;
    _lcd_device.write = RT_NULL;

    _lcd_device.user_data = &lcd_ili_ops;
    //lcd_Initializtion();

    /* register graphic device driver */
    rt_device_register(&_lcd_device, "lcd",
                       RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE);
}
#endif
