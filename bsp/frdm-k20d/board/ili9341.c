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
#include "board.h"
#define printf               rt_kprintf //使用rt_kprintf来输出
//#define printf(...)                       //无输出
unsigned int  HDP=319;
unsigned int  VDP=239;

/* LCD is connected to the FSMC_Bank1_NOR/SRAM2 and NE2 is used as ship select signal */
/* RS <==> A2 */
#define LCD_REG              (*((volatile unsigned short *) 0x60000000)) /* RS = 0 */
#define LCD_RAM              (*((volatile unsigned short *) 0x60000002)) /* RS = 1 */

static void delay(int cnt)
{
    volatile unsigned int dl;
    while(cnt--)
    {
        for(dl=0; dl<500; dl++);
    }
}
lcd_inline void LCD_WR_REG(rt_uint16_t index)
{
	GLCD_CS_L();
	GLCD_RS_L();
	LCD_REG= index;
	GLCD_CS_H();
}

lcd_inline rt_uint16_t LCD_RD_DAT(void)
{
    rt_uint16_t a=0;    
    GLCD_CS_L();
    GLCD_RS_H();
    a=LCD_RAM;    
    GLCD_CS_H();
    return(a);
}

lcd_inline void LCD_WR_DAT(rt_uint16_t val)
{  
	GLCD_CS_L();
	GLCD_RS_H();
	LCD_RAM= val;      
	GLCD_CS_H();
}

rt_uint8_t Read_Register(rt_uint8_t Addr, rt_uint8_t xParameter)
{
    rt_uint8_t data=0;
    LCD_WR_REG(0xd9);                                                      /* ext command                  */
    LCD_WR_DAT(0x10+xParameter);                                        /* 0x11 is the first Parameter  */
    GLCD_CS_L();
    GLCD_RS_L();
    LCD_REG = Addr;
    GLCD_RS_H();
    data = LCD_RAM;
    GLCD_CS_H();
    return data;
}

/********* control <只移植以上函数即可> ***********/

static unsigned short deviceid=0;//设置一个静态变量用来保存LCD的ID

//返回LCD的ID
unsigned int lcd_getdeviceid(void)
{
    rt_uint8_t i=0;
    rt_uint8_t data[3] ;
    rt_uint8_t ID[3] = {0x00, 0x93, 0x41};
    rt_uint8_t ToF=1;
    for(i=0;i<3;i++)
    {
        data[i]=Read_Register(0xd3,i+1);
        if(data[i] != ID[i])
        {
            ToF=0;
        }
    }
    if(!ToF)                                                            /* data!=ID                     */
    {
        rt_kprintf("Read TFT ID failed, ID should be 0x09341, but read ID = 0x");
        for(i=0;i<3;i++)
        {
            rt_kprintf("%d",data[i]);
        }
        rt_kprintf("\r\n");
    }
    deviceid=(data[1]<<8)|data[2];
    return deviceid;
}

void _set_window(rt_uint16_t x1, rt_uint16_t y1, rt_uint16_t x2, rt_uint16_t y2)
{
    LCD_WR_REG(0x002A);
    LCD_WR_DAT(x1>>8);      
    LCD_WR_DAT(x1&0x00ff);
    LCD_WR_DAT(x2>>8);
    LCD_WR_DAT(x2&0x00ff);
    LCD_WR_REG(0x002b);
    LCD_WR_DAT(y1>>8);
    LCD_WR_DAT(y1&0x00ff);
    LCD_WR_DAT(y2>>8);
    LCD_WR_DAT(y2&0x00ff);
}

void _set_cursor(rt_uint16_t x,rt_uint16_t y)
{
    _set_window(x, y, HDP, VDP);
}

void lcd_set_pixel(const char* pixel, int x, int y)
{
    _set_cursor(x, y);
    LCD_WR_REG(0x2c);
    LCD_WR_DAT(*(rt_uint16_t*)pixel);
}

void lcd_get_pixel(char* pixel, int x, int y)
{
    _set_cursor(x, y);
    LCD_WR_REG(0x2e);
    *(rt_uint16_t*)pixel = LCD_RD_DAT();
}

void lcd_draw_hline(const char* pixel, int x1, int x2, int y)
{
    _set_cursor(x1, y);
    LCD_WR_REG(0x2c);
    while (x1 < x2)
    {
        LCD_WR_DAT(*(rt_uint16_t*)pixel);
        x1++;
    }
}

void lcd_draw_vline(const char* pixel, int x, int y1, int y2)
{
    _set_window(x, y1, x, y2);
    LCD_WR_REG(0x2c);
    while (y1 < y2)
    {
        LCD_WR_DAT(*(rt_uint16_t*)pixel);
        y1++;
    }
}

void lcd_blit_line(const char* pixels, int x, int y, rt_size_t size)
{
    rt_uint16_t *ptr;
    ptr = (rt_uint16_t*)pixels;

    _set_cursor(x, y);
    LCD_WR_REG(0x2c);
    while (size)
    {
        LCD_WR_DAT(*ptr ++);
        size --;
    }
}

void lcd_clear()
{                   
    int w = HDP + 1, h = VDP + 1;
    _set_cursor(0, 0);
    LCD_WR_REG(0x2c);
    while (w--)
    {
        while(h--)
        {
            LCD_WR_DAT(0xffff);
        }
        h = VDP + 1;
    }
}

void lcd_Initializtion(void)
{
    lcd_getdeviceid();
    //delay(100000);
    
    LCD_WR_REG(0xCF);  //Power control B
    LCD_WR_DAT(0x00); 
    LCD_WR_DAT(0xC1); 
    LCD_WR_DAT(0X30); 
     
    LCD_WR_REG(0xED);  //Power on sequence control
    LCD_WR_DAT(0x64); 
    LCD_WR_DAT(0x03); 
    LCD_WR_DAT(0X12); 
    LCD_WR_DAT(0X81); 
     
    LCD_WR_REG(0xE8);  //Driver timing control A
    LCD_WR_DAT(0x85); 
    LCD_WR_DAT(0x10); 
    LCD_WR_DAT(0x7A); 
     
    LCD_WR_REG(0xCB);  //Power control A
    LCD_WR_DAT(0x39); 
    LCD_WR_DAT(0x2C); 
    LCD_WR_DAT(0x00); 
    LCD_WR_DAT(0x34); 
    LCD_WR_DAT(0x02); 
     
    LCD_WR_REG(0xF7);  //Pump ratio control
    LCD_WR_DAT(0x20); 
     
    LCD_WR_REG(0xEA);  //Driver timing control B
    LCD_WR_DAT(0x00); 
    LCD_WR_DAT(0x00); 
     
    LCD_WR_REG(0xC0);    //Power control 
    LCD_WR_DAT(0x1B);   //VRH[5:0]  1B
     
    LCD_WR_REG(0xC1);    //Power control 
    LCD_WR_DAT(0x01);   //SAP[2:0];BT[3:0] 
     
    LCD_WR_REG(0xC5);    //VCM control 
    LCD_WR_DAT(0x45);	 //3F
    LCD_WR_DAT(0x25);	 //3C
     
    LCD_WR_REG(0xC7);    //VCM control2 
    LCD_WR_DAT(0XB7);	    //b7
     
    LCD_WR_REG(0x36);    // Memory Access Control 
    LCD_WR_DAT(0x28); 
     
    LCD_WR_REG(0x3A);  //Pixel Format Set
    LCD_WR_DAT(0x55); 
    
    LCD_WR_REG(0xB1);   //Frame Rate Control
    LCD_WR_DAT(0x00);   
    LCD_WR_DAT(0x1A); 
     
    LCD_WR_REG(0xB6);    // Display Function Control 
    LCD_WR_DAT(0x0A); 
    LCD_WR_DAT(0x82); 
    
    LCD_WR_REG(0xF2);    // 3Gamma Function Disable 
    LCD_WR_DAT(0x00); 
     
    LCD_WR_REG(0x26);    //Gamma curve selected 
    LCD_WR_DAT(0x01); 
     
    LCD_WR_REG(0xE0);    //Set Gamma 
    LCD_WR_DAT(0x0F); 
    LCD_WR_DAT(0x2A); 
    LCD_WR_DAT(0x28); 
    LCD_WR_DAT(0x08); 
    LCD_WR_DAT(0x0E); 
    LCD_WR_DAT(0x08); 
    LCD_WR_DAT(0x54); 
    LCD_WR_DAT(0XA9); 
    LCD_WR_DAT(0x43); 
    LCD_WR_DAT(0x0A); 
    LCD_WR_DAT(0x0F); 
    LCD_WR_DAT(0x00); 
    LCD_WR_DAT(0x00); 
    LCD_WR_DAT(0x00); 
    LCD_WR_DAT(0x00); 
     
    LCD_WR_REG(0XE1);    //Set Gamma 
    LCD_WR_DAT(0x00); 
    LCD_WR_DAT(0x15); 
    LCD_WR_DAT(0x17); 
    LCD_WR_DAT(0x07); 
    LCD_WR_DAT(0x11); 
    LCD_WR_DAT(0x06); 
    LCD_WR_DAT(0x2B); 
    LCD_WR_DAT(0x56); 
    LCD_WR_DAT(0x3C); 
    LCD_WR_DAT(0x05); 
    LCD_WR_DAT(0x10); 
    LCD_WR_DAT(0x0F); 
    LCD_WR_DAT(0x3F); 
    LCD_WR_DAT(0x3F); 
    LCD_WR_DAT(0x0F); 
    
    LCD_WR_REG(0x2A);
    LCD_WR_DAT(0x00);
    LCD_WR_DAT(0x00);
    LCD_WR_DAT(0x01);
    LCD_WR_DAT(0x3F);    
    
    LCD_WR_REG(0x2B);
    LCD_WR_DAT(0x00);
    LCD_WR_DAT(0x00);
    LCD_WR_DAT(0x00);
    LCD_WR_DAT(0xEF);
    
    LCD_WR_REG(0x11); //Exit Sleep
    delay(120);
    LCD_WR_REG(0x29); //display on
    LCD_WR_REG(0x2C);
    

    //清屏
    lcd_clear( Blue );
}

struct rt_device_graphic_ops lcd_ili_ops =
{
    lcd_set_pixel,
    lcd_get_pixel,
    lcd_draw_hline,
    lcd_draw_vline,
    lcd_blit_line
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
