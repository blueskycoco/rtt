#include "ili9341.h"


// Compatible list:
// ili9320 ili9325 ili9328
// LG4531

//内联函数定义,用以提高性能
#ifdef __CC_ARM                			 /* ARM Compiler 	*/
#define lcd_inline   				static __inline
#elif defined (__ICCARM__)        		/* for IAR Compiler */
#define lcd_inline 					inline
#elif defined (__GNUC__)        		/* GNU GCC Compiler */
#define lcd_inline 					static __inline
#else
#define lcd_inline                 static
#endif

#define rw_data_prepare()               write_cmd(34)


/********* control ***********/
//#include "stm32f10x.h"
#include "board.h"

//输出重定向.当不进行重定向时.
#define printf               rt_kprintf //使用rt_kprintf来输出
//#define printf(...)                       //无输出

/* LCD is connected to the FSMC_Bank1_NOR/SRAM2 and NE2 is used as ship select signal */
/* RS <==> A2 */
#define LCD_REG              (*((volatile unsigned short *) 0x60000000)) /* RS = 0 */
#define LCD_RAM              (*((volatile unsigned short *) 0x60000002)) /* RS = 1 */

static void LCD_FSMCConfig(void)
{
#if 0
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  Timing_read,Timing_write;

    /* FSMC GPIO configure */
    {
        GPIO_InitTypeDef GPIO_InitStructure;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF
                               | RCC_APB2Periph_GPIOG, ENABLE);
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

        /*
        FSMC_D0 ~ FSMC_D3
        PD14 FSMC_D0   PD15 FSMC_D1   PD0  FSMC_D2   PD1  FSMC_D3
        */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_14 | GPIO_Pin_15;
        GPIO_Init(GPIOD,&GPIO_InitStructure);

        /*
        FSMC_D4 ~ FSMC_D12
        PE7 ~ PE15  FSMC_D4 ~ FSMC_D12
        */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10
                                      | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
        GPIO_Init(GPIOE,&GPIO_InitStructure);

        /* FSMC_D13 ~ FSMC_D15   PD8 ~ PD10 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
        GPIO_Init(GPIOD,&GPIO_InitStructure);

        /*
        FSMC_A0 ~ FSMC_A5   FSMC_A6 ~ FSMC_A9
        PF0     ~ PF5       PF12    ~ PF15
        */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3
                                      | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
        GPIO_Init(GPIOF,&GPIO_InitStructure);

        /* FSMC_A10 ~ FSMC_A15  PG0 ~ PG5 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
        GPIO_Init(GPIOG,&GPIO_InitStructure);

        /* FSMC_A16 ~ FSMC_A18  PD11 ~ PD13 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
        GPIO_Init(GPIOD,&GPIO_InitStructure);

        /* RD-PD4 WR-PD5 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
        GPIO_Init(GPIOD,&GPIO_InitStructure);

        /* NBL0-PE0 NBL1-PE1 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
        GPIO_Init(GPIOE,&GPIO_InitStructure);

        /* NE1/NCE2 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        GPIO_Init(GPIOD,&GPIO_InitStructure);
        /* NE2 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
        GPIO_Init(GPIOG,&GPIO_InitStructure);
        /* NE3 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_Init(GPIOG,&GPIO_InitStructure);
        /* NE4 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
        GPIO_Init(GPIOG,&GPIO_InitStructure);
    }
    /* FSMC GPIO configure */

    /*-- FSMC Configuration -------------------------------------------------*/
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &Timing_read;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &Timing_write;
    FSMC_NORSRAMStructInit(&FSMC_NORSRAMInitStructure);

    Timing_read.FSMC_AddressSetupTime = 3;             /* 地址建立时间  */
    Timing_read.FSMC_DataSetupTime = 4;                /* 数据建立时间  */
    Timing_read.FSMC_AccessMode = FSMC_AccessMode_A;    /* FSMC 访问模式 */

    Timing_write.FSMC_AddressSetupTime = 2;             /* 地址建立时间  */
    Timing_write.FSMC_DataSetupTime = 3;                /* 数据建立时间  */
    Timing_write.FSMC_AccessMode = FSMC_AccessMode_A;   /* FSMC 访问模式 */

    /* Color LCD configuration ------------------------------------
       LCD configured as follow:
          - Data/Address MUX = Disable
          - Memory Type = SRAM
          - Data Width = 16bit
          - Write Operation = Enable
          - Extended Mode = Enable
          - Asynchronous Wait = Disable */
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM2;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM2, ENABLE);
    #endif

}

static void delay(int cnt)
{
    volatile unsigned int dl;
    while(cnt--)
    {
        for(dl=0; dl<500; dl++);
    }
}

static void lcd_port_init(void)
{
    LCD_FSMCConfig();
}
lcd_inline void wr_cmd(unsigned char cmd)
{
	GLCD_CS_L();
	GLCD_RS_L();
	LCD_REG  = cmd;
	GLCD_CS_H();
}

lcd_inline void wr_data(unsigned char dat)
{
	unsigned short _dat = 0x100;
	_dat |= dat;
	GLCD_CS_L();
	GLCD_RS_H();
	LCD_RAM  = dat;
	GLCD_CS_H();
}

lcd_inline void write_cmd(unsigned short cmd)
{
	GLCD_CS_L();
	GLCD_RS_L();
      LCD_REG = cmd;
      GLCD_CS_H();
}

lcd_inline unsigned short read_data(void)
{
	volatile unsigned short data;

	GLCD_CS_L();
	GLCD_RS_H();
      data= LCD_RAM;
      GLCD_CS_H();
	return data;
}

lcd_inline void write_data(unsigned short data_code )
{
	GLCD_CS_L();
	GLCD_RS_H();
      LCD_RAM = data_code;
      GLCD_CS_H();
}

lcd_inline void write_reg(unsigned char reg_addr,unsigned short reg_val)
{
    write_cmd(reg_addr);
    write_data(reg_val);
}

lcd_inline unsigned short read_reg(unsigned char reg_addr)
{
    unsigned short val=0;
    write_cmd(reg_addr);
    val = read_data();
    return (val);
}

/********* control <只移植以上函数即可> ***********/

static unsigned short deviceid=0;//设置一个静态变量用来保存LCD的ID

//返回LCD的ID
unsigned int lcd_getdeviceid(void)
{
    return deviceid;
}

static unsigned short BGR2RGB(unsigned short c)
{
    unsigned short  r, g, b, rgb;

    b = (c>>0)  & 0x1f;
    g = (c>>5)  & 0x3f;
    r = (c>>11) & 0x1f;

    rgb =  (b<<11) + (g<<5) + (r<<0);

    return( rgb );
}

static void lcd_SetCursor(unsigned int x,unsigned int y)
{
    write_reg(32,x);    /* 0-239 */
    write_reg(33,y);    /* 0-319 */
}

/* 读取指定地址的GRAM */
static unsigned short lcd_read_gram(unsigned int x,unsigned int y)
{
    unsigned short temp;
    lcd_SetCursor(x,y);
    rw_data_prepare();
    /* dummy read */
    temp = read_data();
    temp = read_data();
    return temp;
}

static void lcd_clear(unsigned short Color)
{
    unsigned int index=0;
    lcd_SetCursor(0,0);
    rw_data_prepare();                      /* Prepare to write GRAM */
    for (index=0; index<(LCD_WIDTH*LCD_HEIGHT); index++)
    {
        write_data(Color);
    }
}

static void lcd_data_bus_test(void)
{
    unsigned short temp1;
    unsigned short temp2;
    /* [5:4]-ID~ID0 [3]-AM-1垂直-0水平 */
    write_reg(0x0003,(1<<12)|(1<<5)|(1<<4) | (0<<3) );

    /* wirte */
    lcd_SetCursor(0,0);
    rw_data_prepare();
    write_data(0x5555);
    write_data(0xAAAA);

    /* read */
    lcd_SetCursor(0,0);
    if (
        (deviceid ==0x9325)
        || (deviceid ==0x9328)
        || (deviceid ==0x9320)
    )
    {
        temp1 = BGR2RGB( lcd_read_gram(0,0) );
        temp2 = BGR2RGB( lcd_read_gram(1,0) );
    }
    else if( deviceid ==0x4531 )
    {
        temp1 = lcd_read_gram(0,0);
        temp2 = lcd_read_gram(1,0);
    }

    if( (temp1 == 0x5555) && (temp2 == 0xAAAA) )
    {
        printf(" data bus test pass!");
    }
    else
    {
        printf(" data bus test error: %04X %04X",temp1,temp2);
    }
}

void lcd_Initializtion(void)
{
    lcd_port_init();
    deviceid = read_reg(0x00);

    //delay(100000);
    
    wr_cmd(0xCF);  //Power control B
    wr_data(0x00); 
    wr_data(0xC1); 
    wr_data(0X30); 
     
    wr_cmd(0xED);  //Power on sequence control
    wr_data(0x64); 
    wr_data(0x03); 
    wr_data(0X12); 
    wr_data(0X81); 
     
    wr_cmd(0xE8);  //Driver timing control A
    wr_data(0x85); 
    wr_data(0x10); 
    wr_data(0x7A); 
     
    wr_cmd(0xCB);  //Power control A
    wr_data(0x39); 
    wr_data(0x2C); 
    wr_data(0x00); 
    wr_data(0x34); 
    wr_data(0x02); 
     
    wr_cmd(0xF7);  //Pump ratio control
    wr_data(0x20); 
     
    wr_cmd(0xEA);  //Driver timing control B
    wr_data(0x00); 
    wr_data(0x00); 
     
    wr_cmd(0xC0);    //Power control 
    wr_data(0x1B);   //VRH[5:0]  1B
     
    wr_cmd(0xC1);    //Power control 
    wr_data(0x01);   //SAP[2:0];BT[3:0] 
     
    wr_cmd(0xC5);    //VCM control 
    wr_data(0x45);	 //3F
    wr_data(0x25);	 //3C
     
    wr_cmd(0xC7);    //VCM control2 
    wr_data(0XB7);	    //b7
     
    wr_cmd(0x36);    // Memory Access Control 
    wr_data(0x28); 
     
    wr_cmd(0x3A);  //Pixel Format Set
    wr_data(0x55); 
    
    wr_cmd(0xB1);   //Frame Rate Control
    wr_data(0x00);   
    wr_data(0x1A); 
     
    wr_cmd(0xB6);    // Display Function Control 
    wr_data(0x0A); 
    wr_data(0x82); 
    
    wr_cmd(0xF2);    // 3Gamma Function Disable 
    wr_data(0x00); 
     
    wr_cmd(0x26);    //Gamma curve selected 
    wr_data(0x01); 
     
    wr_cmd(0xE0);    //Set Gamma 
    wr_data(0x0F); 
    wr_data(0x2A); 
    wr_data(0x28); 
    wr_data(0x08); 
    wr_data(0x0E); 
    wr_data(0x08); 
    wr_data(0x54); 
    wr_data(0XA9); 
    wr_data(0x43); 
    wr_data(0x0A); 
    wr_data(0x0F); 
    wr_data(0x00); 
    wr_data(0x00); 
    wr_data(0x00); 
    wr_data(0x00); 
     
    wr_cmd(0XE1);    //Set Gamma 
    wr_data(0x00); 
    wr_data(0x15); 
    wr_data(0x17); 
    wr_data(0x07); 
    wr_data(0x11); 
    wr_data(0x06); 
    wr_data(0x2B); 
    wr_data(0x56); 
    wr_data(0x3C); 
    wr_data(0x05); 
    wr_data(0x10); 
    wr_data(0x0F); 
    wr_data(0x3F); 
    wr_data(0x3F); 
    wr_data(0x0F); 
    
    wr_cmd(0x2A);
    wr_data(0x00);
    wr_data(0x00);
    wr_data(0x01);
    wr_data(0x3F);    
    
    wr_cmd(0x2B);
    wr_data(0x00);
    wr_data(0x00);
    wr_data(0x00);
    wr_data(0xEF);
    
    wr_cmd(0x11); //Exit Sleep
    delay(120);
    wr_cmd(0x29); //display on
    wr_cmd(0x2C);
    

    //数据总线测试,用于测试硬件连接是否正常.
    lcd_data_bus_test();

    //清屏
    lcd_clear( Blue );
}

/*  设置像素点 颜色,X,Y */
void rt_hw_lcd_set_pixel(const char* pixel, int x, int y)
{
    lcd_SetCursor(x,y);

    rw_data_prepare();
    write_data(*(rt_uint16_t*)pixel);
}

/* 获取像素点颜色 */
void rt_hw_lcd_get_pixel(char* pixel, int x, int y)
{
    *(rt_uint16_t*)pixel = BGR2RGB( lcd_read_gram(x,y) );
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
    /* LCD RESET */
#if 0
    /* PF10 : LCD RESET */
    {
        GPIO_InitTypeDef GPIO_InitStructure;

       RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);

        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_Init(GPIOF,&GPIO_InitStructure);

        GPIO_ResetBits(GPIOF,GPIO_Pin_10);
        GPIO_SetBits(GPIOF,GPIO_Pin_10);
        /* wait for lcd reset */
        rt_thread_delay(1);
    }
#endif
	

    /* register lcd device */
    _lcd_device.type  = RT_Device_Class_Graphic;
    _lcd_device.init  = lcd_init;
    _lcd_device.open  = lcd_open;
    _lcd_device.close = lcd_close;
    _lcd_device.control = lcd_control;
    _lcd_device.read  = RT_NULL;
    _lcd_device.write = RT_NULL;

    _lcd_device.user_data = &lcd_ili_ops;
    lcd_Initializtion();

    /* register graphic device driver */
    rt_device_register(&_lcd_device, "lcd",
                       RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE);
}
