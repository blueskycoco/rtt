#include <rtthread.h>
#include <string.h>

#define DRV_DEBUG
#define LOG_TAG "drv.lcd"
#include <drv_log.h>

#include <rtdevice.h>
#include <board.h>

/* driver of ili9325 lcd
 *
 */

#define  CS_H	GPIOC->ODR 		|=  (1<<9) //CS -- PC9
#define  CS_L	GPIOC->ODR 		&= ~(1<<9)
#define  RS_H	GPIOC->ODR 		|=  (1<<8) //RS -- PC8
#define  RS_L	GPIOC->ODR 		&= ~(1<<8)
#define	 WR_H	GPIOC->ODR 		|=  (1<<7) //RW -- PC7
#define	 WR_L	GPIOC->ODR 		&= ~(1<<7)
#define  RD_H	GPIOC->ODR 		|=  (1<<6) //RD -- PC6
#define  RD_L	GPIOC->ODR 		&= ~(1<<6)
#define  RES_H	GPIOC->ODR 		|=  (1<<5) //RST-- PC5
#define  RES_L	GPIOC->ODR 		&= ~(1<<5)
#define  PortData				(GPIOB->ODR)

#define LCD_WIDTH           80
#define LCD_HEIGHT          240
#define LCD_BITS_PER_PIXEL  16
#define LCD_BUF_SIZE        (LCD_WIDTH * LCD_HEIGHT * LCD_BITS_PER_PIXEL / 8)
#define LCD_PIXEL_FORMAT    RTGRAPHIC_PIXEL_FORMAT_RGB565
#define LCD_DEVICE(dev) (struct drv_lcd_device *)(dev)

struct drv_lcd_device {
    struct rt_device parent;

    struct rt_device_graphic_info lcd_info;

    rt_uint8_t *frame_buf;
};

struct drv_lcd_device _lcd;

static rt_err_t drv_lcd_init(struct rt_device *device) {
    struct drv_lcd_device *lcd = LCD_DEVICE(device);
    /* nothing, right now */
    lcd = lcd;
    return RT_EOK;
}

static void cmd(uint16_t Cmd) {
	 CS_L;RS_L;RD_H;
	 PortData = Cmd ;
	 WR_L;WR_H;CS_H;
}

static void w_data(uint16_t Data) {
	 CS_L;RS_H;RD_H;
	 PortData = Data ;
	 WR_L;WR_H;CS_H;
}

static rt_uint16_t r_data(void) {
	 rt_uint16_t value;

	 CS_L;RS_H;WR_H;RD_L;
	 GPIOB->MODER = 0X00000000;
	 value = GPIOB->IDR ;
	 GPIOB->MODER = 0X55555555;
	 RD_H;CS_H;
	
	 return value;
}

static void w_cmd_data(rt_uint16_t Cmd, rt_uint16_t Data) {
	cmd(Cmd);
	w_data(Data);
}

static rt_uint16_t r_point(rt_uint16_t x, rt_uint16_t y) {
	rt_uint16_t val;

	w_cmd_data(0x0020,x);
  	w_cmd_data(0x0021,y);
	cmd(0x0022);
	val = r_data();
	val = r_data();

	return val;
}

static rt_uint16_t r_id(void) {
	rt_uint16_t val;
	cmd(0x0000);
	val = r_data();

	return val;
}

void clear_screen(rt_uint16_t color) {
	uint32_t index=0;

	//TFT_SetWindow(0,240,0,320);
	w_cmd_data(0X0020,0);
	w_cmd_data(0X0021,0);
	cmd(0x0022);
	for( index = 0; index < 320 * 240; index++ )
	{
		w_data(color);
	}
}

static void set_window(rt_uint16_t xStart, rt_uint16_t xEnd,
						rt_uint16_t yStart, rt_uint16_t yEnd) {
	w_cmd_data(0X0050,xStart);
	w_cmd_data(0X0051,xEnd);
	w_cmd_data(0X0052,yStart);
	w_cmd_data(0X0053,yEnd);
	w_cmd_data(0X0020,xStart);
	w_cmd_data(0X0021,yStart);
	cmd(0x0022);
}

static rt_err_t drv_lcd_control(struct rt_device *device, int cmd, void *args) {
    struct drv_lcd_device *lcd = LCD_DEVICE(device);

    switch (cmd) {
    case RTGRAPHIC_CTRL_RECT_UPDATE: {
		struct rt_device_rect_info *rect = (struct rt_device_rect_info *)args;

		set_window(rect->x, rect->y, rect->x + rect->width,
									rect->y + rect->height);
        for (int y = rect->y; y <= rect->y + rect->height; y++) {
            for (int x = rect->x; x <= rect->x + rect->width; x++) {
                int location = (x) + (y)*rect->width;
                w_data(_lcd.lcd_info.framebuffer[location]);
			}
		}
	}
    break;

    case RTGRAPHIC_CTRL_GET_INFO: {
        struct rt_device_graphic_info *info = (struct rt_device_graphic_info *)args;

        RT_ASSERT(info != RT_NULL);
        info->pixel_format = lcd->lcd_info.pixel_format;
        info->bits_per_pixel = 16;
        info->width = lcd->lcd_info.width;
        info->height = lcd->lcd_info.height;
        info->framebuffer = lcd->lcd_info.framebuffer;
    }
    break;

    default:
        return -RT_EINVAL;
    }

    return RT_EOK;
}

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops lcd_ops = {
    drv_lcd_init,
    RT_NULL,
    RT_NULL,
    RT_NULL,
    RT_NULL,
    drv_lcd_control
};
#endif

static int _lcd_init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = GPIO_PIN_All;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	CS_L;
	RES_L;
	rt_thread_mdelay(100);
	RES_H;
	rt_thread_mdelay(100);

    rt_kprintf("ili9325 id %04x\n", r_id());

    w_cmd_data(0x0001,0x0100);	  /* Driver Output Contral Register */
	w_cmd_data(0x0002,0x0700);    /* LCD Driving Waveform Contral */
	w_cmd_data(0x0003,0x1030);	  /* Entry Mode setting */

	w_cmd_data(0x0004,0x0000);	  /* Scalling Control register */
	w_cmd_data(0x0008,0x0207);	  /* Display Control 2 */
	w_cmd_data(0x0009,0x0000);	  /* Display Control 3 */
	w_cmd_data(0x000A,0x0000);	  /* Frame Cycle Control */
	w_cmd_data(0x000C,0x0000);	  /* External Display Interface Control 1 */
	w_cmd_data(0x000D,0x0000);    /* Frame Maker Position */
	w_cmd_data(0x000F,0x0000);	  /* External Display Interface Control 2 */
	rt_thread_mdelay(50);
	w_cmd_data(0x0007,0x0101);	  /* Display Control */
	rt_thread_mdelay(50);
	w_cmd_data(0x0010,0x16B0);    /* Power Control 1 */
	w_cmd_data(0x0011,0x0001);    /* Power Control 2 */
	w_cmd_data(0x0017,0x0001);    /* Power Control 3 */
	w_cmd_data(0x0012,0x0138);    /* Power Control 4 */
	w_cmd_data(0x0013,0x0800);    /* Power Control 5 */
	w_cmd_data(0x0029,0x0009);	  /* NVM read data 2 */
	w_cmd_data(0x002a,0x0009);	  /* NVM read data 3 */
	w_cmd_data(0x00a4,0x0000);
	w_cmd_data(0x0050,0x0000);
	w_cmd_data(0x0051,0x00EF);
	w_cmd_data(0x0052,0x0000);
	w_cmd_data(0x0053,0x013F);

	w_cmd_data(0x0060,0xA700);	  /* Driver Output Control */
	w_cmd_data(0x0061,0x0003);	  /* Driver Output Control */
	w_cmd_data(0x006A,0x0000);	  /* Vertical Scroll Control */

	w_cmd_data(0x0080,0x0000);	  /* Display Position ????Partial Display 1 */
	w_cmd_data(0x0081,0x0000);	  /* RAM Address Start ????Partial Display 1 */
	w_cmd_data(0x0082,0x0000);	  /* RAM address End - Partial Display 1 */
	w_cmd_data(0x0083,0x0000);	  /* Display Position ????Partial Display 2 */
	w_cmd_data(0x0084,0x0000);	  /* RAM Address Start ????Partial Display 2 */
	w_cmd_data(0x0085,0x0000);	  /* RAM address End ????Partail Display2 */
	w_cmd_data(0x0090,0x0013);	  /* Frame Cycle Control */
	w_cmd_data(0x0092,0x0000); 	  /* Panel Interface Control 2 */
	w_cmd_data(0x0093,0x0003);	  /* Panel Interface control 3 */
	w_cmd_data(0x0095,0x0110);	  /* Frame Cycle Control */
	w_cmd_data(0x0007,0x0173);
  	rt_thread_mdelay(50);   /* delay 50 ms */
  	clear_screen(0x8700);
}

int lcd_init(void)
{
    rt_err_t result = RT_EOK;
    struct rt_device *device = &_lcd.parent;

    /* memset _lcd to zero */
    memset(&_lcd, 0x00, sizeof(_lcd));

    /* config LCD dev info */
    _lcd.lcd_info.height = LCD_HEIGHT;
    _lcd.lcd_info.width = LCD_WIDTH;
    _lcd.lcd_info.bits_per_pixel = LCD_BITS_PER_PIXEL;
    _lcd.lcd_info.pixel_format = LCD_PIXEL_FORMAT;

    /* malloc memory for Triple Buffering */
    _lcd.lcd_info.framebuffer = rt_malloc_align(LCD_BUF_SIZE, LCD_BUF_SIZE);
    if (_lcd.lcd_info.framebuffer == RT_NULL) {
        LOG_E("init frame buffer failed!\n");
        result = -RT_ENOMEM;
        goto __exit;
    }

    /* memset buff to 0xFF */
    memset(_lcd.lcd_info.framebuffer, 0xFF, LCD_BUF_SIZE);

    device->type = RT_Device_Class_Graphic;
#ifdef RT_USING_DEVICE_OPS
    device->ops = &lcd_ops;
#else
    device->init = drv_lcd_init;
#ifndef ART_PI_TouchGFX_LIB
    device->control = drv_lcd_control;
#endif
#endif

    /* register lcd device */
    rt_device_register(device, "lcd", RT_DEVICE_FLAG_RDWR);

	_lcd_init();

__exit:
    if (result != RT_EOK) {
        if (_lcd.lcd_info.framebuffer)
            rt_free(_lcd.lcd_info.framebuffer);
    }
    return result;
}
INIT_DEVICE_EXPORT(lcd_init);
