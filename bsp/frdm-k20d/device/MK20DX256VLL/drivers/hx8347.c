//////////////////////////////////////////////////////////////////////////////
#define _GLCD_C_
//////////////////////////////////////////////////////////////////////////////

/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2010 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/
#include "glcd.h"
#include "font.h"
#include "gpio.h"
#include "flexbus.h"

 
/************************** Orientation  configuration ************************/

#define HORIZONTAL  0                   /* If vertical = 0, if horizontal = 1 */

/*********************** Hardware specific configuration **********************/

/*---------------------- Graphic LCD size definitions ------------------------*/

#if (HORIZONTAL == 1)
#define WIDTH       240                 /* Screen Width (in pixels)           */
#define HEIGHT      320                 /* Screen Hight (in pixels)           */
#else
#define WIDTH       320                 /* Screen Width (in pixels)           */
#define HEIGHT      240                 /* Screen Hight (in pixels)           */
#endif
#define BPP         16                  /* Bits per pixel                     */
#define BYPP        ((BPP+7)/8)         /* Bytes per pixel                    */

/*--------------- Graphic LCD interface hardware definitions -----------------*/

/*---------------------------- Global variables ------------------------------*/

/******************************************************************************/
unsigned short TextColor = Black;
unsigned short BackColor = White;

/*******************************************************************************
* Write a command the LCD controller                                           *
*   Parameter:    cmd:    command to be written                                *
*   Return:                                                                    *
*******************************************************************************/

static __inline void wr_cmd (unsigned short cmd) 
{
	GLCD_RS_L();
	*(volatile unsigned short *)CS0_BASE  = cmd;
}


/*******************************************************************************
* Write data to the LCD controller                                             *
*   Parameter:    dat:    data to be written                                   *
*   Return:                                                                    *
*******************************************************************************/

static __inline void wr_dat (unsigned short dat) 
{
	GLCD_RS_H();
	*(volatile unsigned short *)(CS0_BASE+2)  = dat;
}


/*******************************************************************************
* Start of data writing to the LCD controller                                  *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

static __inline void wr_dat_start (void) 
{
	GLCD_RS_H();
}


/*******************************************************************************
* Stop of data writing to the LCD controller                                   *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

static __inline void wr_dat_stop (void) 
{
	;
}


/*******************************************************************************
* Data writing to the LCD controller                                           *
*   Parameter:    dat:    data to be written                                   *
*   Return:                                                                    *
*******************************************************************************/

static __inline void wr_dat_only (unsigned short dat) 
{
	GLCD_RS_H();
	*(unsigned short *)(CS0_BASE+2) = dat;
}


/*******************************************************************************
* Read data from the LCD controller                                            *
*   Parameter:                                                                 *
*   Return:               read data                                            *
*******************************************************************************/

static __inline unsigned short rd_dat (void) 
{
	unsigned short val = 0;
	GLCD_RS_H();
	val = *(unsigned short *)(CS0_BASE+2);
	return (val);
}


/*******************************************************************************
* Write a value to the to LCD register                                         *
*   Parameter:    reg:    register to be written                               *
*                 val:    value to write to the register                       *
*******************************************************************************/

static __inline void wr_reg (unsigned char reg, unsigned short val) 
{
 	wr_cmd(reg);
 	wr_dat(val);
}


/*******************************************************************************
* Read from the LCD register                                                   *
*   Parameter:    reg:    register to be read                                  *
*   Return:               value read from the register                         *
*******************************************************************************/

static unsigned short rd_reg (unsigned char reg) 
{
 	wr_cmd(reg);
 	return(rd_dat());
}


/************************ Exported functions **********************************/

/*******************************************************************************
* Initialize the Graphic LCD controller                                        *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

unsigned char GLCD_Init (void) 
{ 
	unsigned char LCD_Exist;
	
	GLCD_CS_L();
	LCD_Exist = (rd_reg(0x00) == 0x75) ? 1 : 0;

// Start Initial Sequence (LCD_8347G)---------------------------------------
 	if (LCD_Exist) {
		//Driving ability setting
		wr_reg(0x2E,0x89); //GDOFF
		wr_reg(0x29,0x8F); //RTN
		wr_reg(0x2B,0x06); //DUM
		wr_reg(0xE2,0x00); //VREF
		wr_reg(0xE4,0x01); //EQ
		wr_reg(0xE5,0x10); //EQ
		wr_reg(0xE6,0x01); //EQ
		wr_reg(0xE7,0x10); //EQ
		wr_reg(0xE8,0x70); //OPON
		wr_reg(0xF2,0x00); //GEN
		wr_reg(0xEA,0x00); //PTBA
		wr_reg(0xEB,0x20); //PTBA
		wr_reg(0xEC,0x3C); //STBA
		wr_reg(0xED,0xC8); //STBA
		wr_reg(0xE9,0x38); //OPON1
		wr_reg(0xF1,0x01); //OTPS1B
		//Adjust the Gamma Curve
		wr_reg(0x40,0x00);
		wr_reg(0x41,0x00);
		wr_reg(0x42,0x00);
		wr_reg(0x43,0x14);
		wr_reg(0x44,0x12);
		wr_reg(0x45,0x3F);
		wr_reg(0x46,0x00);
		wr_reg(0x47,0x54);
		wr_reg(0x48,0x00);
		wr_reg(0x49,0x10);
		wr_reg(0x4A,0x17);
		wr_reg(0x4B,0x18);
		wr_reg(0x4C,0x16);
		wr_reg(0x50,0x00);
		wr_reg(0x51,0x2D);
		wr_reg(0x52,0x2B);
		wr_reg(0x53,0x3F);
		wr_reg(0x54,0x3F);
		wr_reg(0x55,0x3F);
		wr_reg(0x56,0x2B);
		wr_reg(0x57,0x7F);
		wr_reg(0x58,0x09);
		wr_reg(0x59,0x07);
		wr_reg(0x5A,0x08);
		wr_reg(0x5B,0x0F);
		wr_reg(0x5C,0x1F);
		wr_reg(0x5D,0xFF);
//Power voltage setting
		wr_reg(0x1B,0x1B);
		wr_reg(0x1A,0x02);	
		wr_reg(0x24,0x61);
		wr_reg(0x25,0x5C);
//VCOM offset
		wr_reg(0x23,0x38);
//Power on setting
		wr_reg(0x18,0x36); 		//RADJ 70Hz
		wr_reg(0x19,0x01); 		//OSC_EN=1
		wr_reg(0x1F,0x88); 		// GAS=1, VOMG=00, PON=0, DK=1, XDK=0, DVDH_TRI=0, STB=0

		wr_reg(0x1F,0x80); 		// GAS=1, VOMG=00, PON=0, DK=0, XDK=0, DVDH_TRI=0, STB=0

		wr_reg(0x1F,0x90); 		// GAS=1, VOMG=00, PON=1, DK=0, XDK=0, DVDH_TRI=0, STB=0

		wr_reg(0x1F,0xD4); 		// GAS=1, VOMG=10, PON=1, DK=0, XDK=1, DDVDH_TRI=0, STB=0

//262k/65k color selection
		wr_reg(0x17,0x05); 		//default 0x06 262k color // 0x05 65k color
//SET PANEL
		wr_reg(0x36,0x09); 		//SS_P, GS_P,REV_P,BGR_P
		wr_reg(0x28,0x38); 		//GON=1, DTE=1, D=1000

		wr_reg(0x28,0x3C); 		//GON=1, DTE=1, D=1100
//Set GRAM Area - Partial Display Control 
		wr_reg(0x01,0x00);
		wr_reg(0x02,0x00);
		wr_reg(0x03,0x00); 		//Column Start
		wr_reg(0x04,0x01);
		wr_reg(0x05,0x3F); 		//Column End
		wr_reg(0x06,0x00);
		wr_reg(0x07,0x00); 		//Row Start
		wr_reg(0x08,0x00);
		wr_reg(0x09,0xEF); 		//Row End
// bit5 controls X,Y swap
		wr_reg(0x16, 0x60); 			// 
		GLCD_Clear(White);		//clear
		GLCD_CS_H();
		return (0);
	}
	else {
		GLCD_CS_H();
		return (1);
	}
}



/*******************************************************************************
* Set draw window region                                                       *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*                   w:        window width in pixel                            *
*                   h:        window height in pixels                          *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_SetWindow (unsigned int x, unsigned int y, unsigned int w, unsigned int h) 
{
	GLCD_CS_L();
	wr_reg(0x02, x>>8);
	wr_reg(0x03, x&0xFF);
	wr_reg(0x04, (x+w-1)>>8);
	wr_reg(0x05, (x+w-1)&0xFF);
	wr_reg(0x06, y>>8);
	wr_reg(0x07, y&0xFF);
	wr_reg(0x08, (y+h-1)>>8);
	wr_reg(0x09, (y+h-1)&0xFF);
	GLCD_CS_H();
}


/*******************************************************************************
* Set draw window region to whole screen                                       *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_WindowMax (void) 
{
	GLCD_CS_L();
#if (HORIZONTAL == 1)
	GLCD_SetWindow (0, 0, HEIGHT, WIDTH);
#else
	GLCD_SetWindow (0, 0, WIDTH,  HEIGHT);
#endif
	GLCD_CS_H();
}


/*******************************************************************************
* Draw a pixel in foreground color                                             *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_DrawPixel (unsigned short x, unsigned short y, unsigned short color) 
{
	GLCD_CS_L();
#if (HORIZONTAL == 1)
	wr_reg(0x06, (WIDTH-x) >> 8);
	wr_reg(0x07, (WIDTH-x) & 0xFF);
	wr_reg(0x02, y >> 8);
	wr_reg(0x03, y & 0xFF);
#else
	wr_reg(0x02, x >> 8);
	wr_reg(0x03, x & 0xFF);
	wr_reg(0x06, y >> 8);
	wr_reg(0x07, y & 0xFF);
#endif
	wr_cmd(0x22);
	
	wr_dat(color);
	GLCD_CS_H();
}

void GLCD_DrawRec (unsigned short x, unsigned short y, unsigned short w, unsigned short h, unsigned short color)
{
	unsigned short i;
	GLCD_SetWindow (x, y, w, h);
	GLCD_CS_L();
	wr_cmd(0x22);
	wr_dat_start();
	for (i=0; i < (w*h); i++){
		wr_dat_only(color);
	}
	wr_dat_stop();
	GLCD_WindowMax();
	GLCD_CS_H();
}
/*******************************************************************************
* Set foreground color                                                         *
*   Parameter:      color:    foreground color                                 *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_SetTextColor (unsigned short color) 
{
	TextColor = color;
}

unsigned short GLCD_GetTextColor (void) 
{
	return TextColor;
}
/*******************************************************************************
* Set background color                                                         *
*   Parameter:      color:    background color                                 *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_SetBackColor (unsigned short color) 
{
	BackColor = color;
}

unsigned short GLCD_GetBackColor (void) 
{
	return BackColor;
}


/*******************************************************************************
* Clear display                                                                *
*   Parameter:      color:    display clearing color                           *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_Clear (unsigned short color) 
{
	unsigned int i;
	GLCD_CS_L();
	wr_reg(0x06, 0);
	wr_reg(0x07, 0);
	wr_reg(0x02, 0);
	wr_reg(0x03, 0);
	wr_cmd(0x22);
	wr_dat_start();
	for(i = 0; i < (WIDTH*HEIGHT); i++)
		wr_dat_only(color);
	wr_dat_stop();
	GLCD_CS_H();
}


/*******************************************************************************
* Draw character on given position                                             *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*                   cw:       character width in pixel                         *
*                   ch:       character height in pixels                       *
*                   c:        pointer to character bitmap                      *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_DrawChar_6x8(unsigned int x, unsigned int y, unsigned char bc, char c) 
{
	int idx = 0, i, j;
	if (0x1F < c && c < 0x80) {
		idx = (c - 0x20)*8;
		for (i = 0;i < 8;i++) {
			for (j = 0; j < 6; j++) {
				if (Font_6x8_h[idx+i] & 0x01<<(7-j)) {
					GLCD_DrawPixel(x+j, y+i, TextColor);
				}
				else {
					if (bc) {
						GLCD_DrawPixel(x+j, y+i, BackColor);
					}
				}
			}
		}
	}
}

/*******************************************************************************
* Draw character on given position                                             *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*                   cw:       character width in pixel                         *
*                   ch:       character height in pixels                       *
*                   c:        pointer to character bitmap                      *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_DrawChar_8x12(unsigned int x, unsigned int y, unsigned char bc, char c) 
{
	int idx = 0, i, j;
	if (0x1F<c && c<0x80){
		idx = (c - 0x20)*12;
		for (i=0;i<12;i++){
			for (j=0;j<8;j++){
				if (Font_8x12_h[idx+i] & 0x01<<(7-j)){
					GLCD_DrawPixel(x+j, y+i, TextColor);
				}
				else{
					if (bc) {
						GLCD_DrawPixel(x+j, y+i, BackColor);
					}
				}
			}
		}
	}
}
/*******************************************************************************
* Draw character on given position                                             *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*                   cw:       character width in pixel                         *
*                   ch:       character height in pixels                       *
*                   c:        pointer to character bitmap                      *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_DrawChar_8x16(unsigned int x, unsigned int y, unsigned char bc, char c) 
{
	int idx = 0, i, j;
	if (0x1F<c && c<0x80){
		idx = (c - 0x20)*16;
		for (i=0;i<16;i++) {
			for (j=0;j<8;j++){
				if (Font_8x16_h[idx+i] & 0x01<<(7-j)){
					GLCD_DrawPixel(x+j, y+i, TextColor);
				}
				else{
					if (bc) {
						GLCD_DrawPixel(x+j, y+i, BackColor);
					}
				}
			}
		}
	}
}
/*******************************************************************************
* Draw character on given position                                             *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*                   cw:       character width in pixel                         *
*                   ch:       character height in pixels                       *
*                   c:        pointer to character bitmap                      *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_DrawChar_16x24(unsigned int x, unsigned int y, unsigned char bc, char c) 
{
	int idx = 0, i, j;
	if (0x1F<c && c<0x80) {
		idx = (c-0x20)*24;
		for (i = 0; i < 24; i++) {
			for (j = 0; j < 16; j++) {
				if (Font_16x24_h[idx + i] & (0x0001 << j)) {
					GLCD_DrawPixel(x+j, y+i, TextColor);
				}
				else {
					if (bc) {
						GLCD_DrawPixel(x+j, y+i, BackColor);
					}
				}
			}
		}
	}
}

/*******************************************************************************
* Disply character on given line                                               *
*   Parameter:      ln:       line number                                      *
*                   col:      column number                                    *
*                   fi:       font index (0 = 6x8, 1 = 16x24)                  *
*                   c:        ascii character                                  *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_DisplayChar (unsigned int x, unsigned int y, unsigned char fi, unsigned char bc, char  c) 
{
	if (c & 0x80)									//is not ascii
		return;
	switch (fi) {
    case 0:  /* Font 6 x 8 */
		GLCD_DrawChar_6x8  (x, y, bc, c);
		break;
    case 1:  /* Font 8 x 12 */
		GLCD_DrawChar_8x12 (x, y, bc, c);
		break;
    case 2:  /* Font 8 x 16 */
		GLCD_DrawChar_8x16 (x, y, bc, c);
		break;
    case 3:  /* Font 16 x 24 */
		GLCD_DrawChar_16x24(x, y, bc, c);
		break;
	default:
		break;
	}
}
/*******************************************************************************
* Disply string on given line                                                  *
*   Parameter:      ln:       line number                                      *
*                   col:      column number                                    *
*                   fi:       font index (0 = 6x8, 1 = 16x24)                  *
*                   s:        pointer to string                                *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_DisplayString	(unsigned int x, unsigned int y, unsigned char fi, unsigned char bc, char *s) 
{
//  GLCD_WindowMax();
	while (*s) {
		switch(fi) {
			case 0:
				GLCD_DisplayChar(x, y, fi, bc, *s++);
				x += 6;
				break;
			case 3:
				GLCD_DisplayChar(x, y, fi, bc, *s++);
				x += 16;
				break;
			default:
				GLCD_DisplayChar(x, y, fi, bc, *s++);
				x += 8;
				break;
		}
	}
}

/*******************************************************************************
* Clear given line                                                             *
*   Parameter:      ln:       line number                                      *
*                   fi:       font index (0 = 6x8, 1 = 16x24)                  *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_ClearLn (unsigned int ln, unsigned char fi) 
{
	unsigned char i;
	char buf[60];

	GLCD_WindowMax();
	switch (fi) {
		case 0:  /* Font 6 x 8 */
			for (i = 0; i < (WIDTH+5)/6; i++)
			buf[i] = ' ';
			buf[i+1] = 0;
		break;
		case 1:  /* Font 16 x 24 */
			for (i = 0; i < (WIDTH+15)/16; i++)
			buf[i] = ' ';
			buf[i+1] = 0;
		break;
	}
	GLCD_DisplayString (ln, 0, fi, 1, buf);
}

/*******************************************************************************
* Draw bargraph                                                                *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*                   w:        maximum width of bargraph (in pixels)            *
*                   val:      value of active bargraph (in 1/1024)             *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_Bargraph (unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int val) 
{
	int i,j;
	GLCD_CS_L();
	val = (val * w) >> 10;                /* Scale value                        */
#if (HORIZONTAL == 1)
	x = WIDTH-x-w;
	GLCD_SetWindow(y, x, h, w);
#else
	GLCD_SetWindow(x, y, w, h);
#endif
	wr_cmd(0x22);
	wr_dat_start();
	for (i = 0; i < h; i++) {
#if (HORIZONTAL == 1)
		for (j = w-1; j >= 0; j--) {
#else
		for (j = 0; j <= w-1; j++) {
#endif
			if(j >= val) 
				wr_dat_only(BackColor);
			else 
				wr_dat_only(TextColor);
		}
	}
	wr_dat_stop();
	GLCD_CS_H();
}


/*******************************************************************************
* Display graphical bitmap image at position x horizontally and y vertically   *
* (This function is optimized for 16 bits per pixel format, it has to be       *
*  adapted for any other bits per pixel format)                                *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*                   w:        width of bitmap                                  *
*                   h:        height of bitmap                                 *
*                   bitmap:   address at which the bitmap data resides         *
*   Return:                                                                    *
*******************************************************************************/

// void GLCD_Bitmap (unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char *bitmap) 
// {
// 	unsigned int    i, j;
// 	unsigned short *bitmap_ptr = (unsigned short *)bitmap;

// 	for (i=y;i<y+h;i++)
// 	{
// 		for (j=x;j<x+w;j++)
// 		{
// 			GLCD_DrawPixel (j, i, *bitmap_ptr++);
// 		}
// 	}
// }									// 打点方式，最普遍做法

void GLCD_Bitmap (unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char *bitmap) 
{
	unsigned int i;
	unsigned short *bitmap_ptr = (unsigned short *)bitmap;
	GLCD_SetWindow (x, y, w, h);
	GLCD_CS_L();
 	wr_cmd(0x22);
	wr_dat_start();
	for(i = 0; i < (WIDTH*HEIGHT); i++)
		wr_dat_only(*bitmap_ptr++);
	wr_dat_stop();
	GLCD_CS_H();
}										// 优化，通过setwindow 方式，在屏幕开窗口填数据

/*******************************************************************************
* Scroll content of the whole display for dy pixels vertically                 *
*   Parameter:      dy:       number of pixels for vertical scroll             *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_ScrollVertical (unsigned int dy) 
{
#if (HORIZONTAL == 0)
	static unsigned int y = 0;
	GLCD_CS_L();
	y = y + dy;
	while (y >= HEIGHT) 
		y -= HEIGHT;

	wr_reg(0x6A, y);
	wr_reg(0x61, 3);
	GLCD_CS_H();
#endif
}

/******************************************************************************/

