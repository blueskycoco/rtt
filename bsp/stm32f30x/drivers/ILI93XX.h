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


void LCD_Write_Cmd(uint16_t Cmd);									/*дָ��*/
void LCD_Write_Data(uint16_t Data);									/*д����*/
uint16_t LCD_Read_Data(void);												/*��ȡ����*/
uint16_t Read_Point(uint16_t x,uint16_t y);					/*��ȡ��*/
uint16_t Read_ID(void);														/*��ȡҺ������ID*/
void Write_Cmd_Data(uint16_t Cmd, uint16_t Data);						/*д���дָ��*/
void TFT_Init(void);	              								/*Һ��������ʼ��*/

void LCD_Clear(uint16_t Color);	  									/*����*/
void SetWindow(unsigned int X_Start,unsigned int X_End,unsigned int Y_Start,unsigned int Y_End);
void LCD_SetPoint(uint16_t Xpos,uint16_t Ypos,uint16_t point);
void Draw_Pixel(uint16_t X,uint16_t Y,uint16_t color);				//��������
void LCD_PutChar(unsigned short x, unsigned short y, char c, unsigned int fColor, unsigned int bColor);
#endif 
