////////////////////////////////////////////////////////////////////////////////
#ifndef _GLCD_H
#define _GLCD_H
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/* GLCD.h: Graphic LCD function prototypes and defines                        */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2009 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#define RS_MODE				0				//1:IO software RS 0:ADDR1 hardware RS

#if RS_MODE == 1
	#define GLCD_RS_H()		GPIO_SetBit(PTC, IO_16)
	#define GLCD_RS_L()		GPIO_ClrBit(PTC, IO_16)
#else
	#define GLCD_RS_H()
	#define GLCD_RS_L()
#endif

#define GLCD_CS_H()			GPIO_SetBit(PTD, IO_1)
#define GLCD_CS_L()			GPIO_ClrBit(PTD, IO_1)
/*------------------------------------------------------------------------------
  Color coding
  GLCD is coded:   15..11 red, 10..5 green, 4..0 blue  (unsigned short)  GLCD_R5, GLCD_G6, GLCD_B5   
  original coding: 17..12 red, 11..6 green, 5..0 blue                    ORG_R6,  ORG_G6,  ORG_B6

  ORG_R1..5 = GLCD_R0..4,  ORG_R0 = GLCD_R4
  ORG_G0..5 = GLCD_G0..5,
  ORG_B1..5 = GLCD_B0..4,  ORG_B0 = GLCD_B4
 *----------------------------------------------------------------------------*/
                            
/* GLCD RGB color definitions                                                 */
#define Black           0x0000      /*   0,   0,   0 */
#define Navy            0x000F      /*   0,   0, 128 */
#define DarkGreen       0x03E0      /*   0, 128,   0 */
#define DarkCyan        0x03EF      /*   0, 128, 128 */
#define Maroon          0x7800      /* 128,   0,   0 */
#define Purple          0x780F      /* 128,   0, 128 */
#define Olive           0x7BE0      /* 128, 128,   0 */
#define LightGrey       0xC618      /* 192, 192, 192 */
#define DarkGrey        0x7BEF      /* 128, 128, 128 */
#define Blue            0x001F      /*   0,   0, 255 */
#define Green           0x07E0      /*   0, 255,   0 */
#define Cyan            0x07FF      /*   0, 255, 255 */
#define Red             0xF800      /* 255,   0,   0 */
#define Magenta         0xF81F      /* 255,   0, 255 */
#define Yellow          0xFFE0      /* 255, 255, 0   */
#define White           0xFFFF      /* 255, 255, 255 */

#define Line0           ( 0*24)
#define Line1           ( 1*24)
#define Line2           ( 2*24)
#define Line3           ( 3*24)
#define Line4           ( 4*24)
#define Line5           ( 5*24)
#define Line6           ( 6*24)
#define Line7           ( 7*24)
#define Line8           ( 8*24)
#define Line9           ( 9*24)

unsigned short GLCD_GetTextColor (void);
unsigned short GLCD_GetBackColor (void);
unsigned char  GLCD_Init	     (void);
void GLCD_WindowMax      	(void);
void GLCD_DrawPixel 		(unsigned short x, unsigned short y, unsigned short color);
void GLCD_SetTextColor   	(unsigned short color);
void GLCD_SetBackColor   	(unsigned short color);
void GLCD_DrawRec 			(unsigned short x, unsigned short y, unsigned short w, unsigned short h, unsigned short color);
void GLCD_Clear          	(unsigned short color);
void GLCD_DisplayChar		(unsigned int x, unsigned int y, unsigned char fi, unsigned char bc, char  c);
void GLCD_DisplayString		(unsigned int x, unsigned int y, unsigned char fi, unsigned char bc, char *s);
void GLCD_ClearLn        	(unsigned int ln, unsigned char fi);
void GLCD_Bargraph       	(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int val);
void GLCD_Bitmap         	(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char *bitmap);
void GLCD_ScrollVertical 	(unsigned int dy);

////////////////////////////////////////////////////////////////////////////////
#endif /* _GLCD_H */
////////////////////////////////////////////////////////////////////////////////
