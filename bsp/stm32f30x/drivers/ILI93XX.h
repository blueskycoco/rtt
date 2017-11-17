#ifndef __ILI93XX_H
#define __ILI93XX_H

#include "STM32F30x.h"
#include "app_types.h"

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

void LCD_SetPoint(uint16_t Xpos,uint16_t Ypos,uint16_t point);
void LCD_PutChar(unsigned short x, unsigned short y, char c, unsigned int fColor, unsigned int bColor);
#define TFT_XMAX                BOARD_LCD_WIDTH                       //设置TFT屏的大小
#define TFT_YMAX                BOARD_LCD_HEIGHT


#define TFT_WriteCmdEXT         TFT_WriteCmd
#define TFT_WriteDataEXT        TFT_WriteData
#define TFT_SetWindowEXT        TFT_SetWindow


void TFT_Init(void);

void TFT_ClearScreen(uint16 color);

uint16  TFT_Read(void);

void  TFT_WriteCmd(uint16 cmd);
void  TFT_WriteData(uint16 dat);
void  TFT_SetWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd);

void  GUI_DispColor(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend, unsigned int color);
void  GUI_Point(uint16 x, uint16 y, uint16 color);
#endif 
