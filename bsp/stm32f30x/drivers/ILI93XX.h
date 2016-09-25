#ifndef __ILI93XX_H
#define __ILI93XX_H

#include "STM32F30x.h"


/* LCD color */
#define White          0xFFFF
#define Black          0x0000
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0


void LCD_Write_Cmd(uint16_t Cmd);									/*写指令*/
void LCD_Write_Data(uint16_t Data);									/*写数据*/
uint16_t LCD_Read_Data(void);												/*读取数据*/
uint16_t Read_Point(uint16_t x,uint16_t y);					/*读取点*/
uint16_t Read_ID(void);														/*读取液晶驱动ID*/
void Write_Cmd_Data(uint16_t Cmd, uint16_t Data);						/*写命令，写指令*/
void TFT_Init(void);	              								/*液晶驱动初始化*/

void LCD_Clear(uint16_t Color);	  									/*清屏*/
void SetWindow(unsigned int X_Start,unsigned int X_End,unsigned int Y_Start,unsigned int Y_End);
void LCD_SetPoint(uint16_t Xpos,uint16_t Ypos,uint16_t point);
void Draw_Pixel(uint16_t X,uint16_t Y,uint16_t color);				//触摸画点
void LCD_PutChar(unsigned short x, unsigned short y, char c, unsigned int fColor, unsigned int bColor);
#endif 
