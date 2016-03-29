#include "GLCD.h"

#include "font.h"
#include "gpio.h"
#include "flexbus.h"
 
/************************** Orientation  configuration ************************/

#define HORIZONTAL  0                   /* If vertical = 0, if horizontal = 1 */

/*---------------------- Graphic LCD size definitions ------------------------*/

#if (HORIZONTAL == 1)
#define WIDTH       240                 /* Screen Width (in pixels)           */
#define HEIGHT      320                 /* Screen Hight (in pixels)           */
#else
#define WIDTH       320                 /* Screen Width (in pixels)           */
#define HEIGHT      240                 /* Screen Hight (in pixels)           */
#endif

#define GLCD_BACKCOLOR			White 

/******************************************************************************/
static unsigned short TextColor = Black;
static unsigned short BackColor = White;

static void delay(int count)
{
  while(count != 0) count--;
  return;
}

static __inline void wr_cmd(unsigned char cmd)
{
	GLCD_CS_L();
	GLCD_RS_L();
	*(volatile unsigned short *)CS0_BASE  = cmd;
	GLCD_CS_H();
}

static __inline void wr_data(unsigned char dat)
{
	unsigned short _dat = 0x100;
	_dat |= dat;
	GLCD_CS_L();
	GLCD_RS_H();
	*(volatile unsigned short *)(CS0_BASE+2)  = dat;
	GLCD_CS_H();
}

static __inline void wr_data16(unsigned short dat)
{	
	GLCD_CS_L();
	GLCD_RS_H();
	*(volatile unsigned short *)(CS0_BASE+2)  = dat;
	GLCD_CS_H();
}

static __inline void Color_Dat_start(void)
{
	wr_cmd(0x2c);
}

void Set_Address(unsigned short x,unsigned short y)
{
	wr_cmd(0x20);			//AC Conuter Setting 
	wr_data16(x);
	wr_cmd(0x21);
	wr_data16(y);
	wr_cmd (0x22);	
}

unsigned char GLCD_Init (void) 
{ 
	delay(100000);
	
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
	wr_data(0x45); 	   //3F
	wr_data(0x25); 	   //3C
	 
	wr_cmd(0xC7);    //VCM control2 
	wr_data(0XB7); 		//b7
	 
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
	
	GLCD_Clear(GLCD_BACKCOLOR);
	
	return 0;
}

void GLCD_SetWindow (unsigned int x, unsigned int y, unsigned int w, unsigned int h) 
{
	wr_cmd(0x2A);
	wr_data(x>>8);
	wr_data(x&0xFF);
	wr_data((x+w-1)>>8);
	wr_data((x+w-1)&0xFF);	

	wr_cmd(0x2B);
	wr_data(y>>8);
	wr_data(y&0xFF);
	wr_data((y+h-1)>>8);
	wr_data((y+h-1)&0xFF);
}

void GLCD_WindowMax (void) 
{
#if (HORIZONTAL == 1)
	GLCD_SetWindow (0, 0, HEIGHT, WIDTH);
#else
	GLCD_SetWindow (0, 0, WIDTH,  HEIGHT);
#endif

}

void GLCD_DrawPixel (INT16U x, INT16U y, INT16U color) 
{
#if (HORIZONTAL == 1)
	wr_cmd(0x2A);
	wr_data(y>>8);
	wr_data(y&0xFF);;	

	wr_cmd(0x2B);
	wr_data(x>>8);
	wr_data(x&0xFF);
#else
	wr_cmd(0x2A);
	wr_data(x>>8);
	wr_data(x&0xFF);

	wr_cmd(0x2B);
	wr_data(y>>8);
	wr_data(y&0xFF);
#endif
	wr_cmd(0x2c);
	wr_data16(color);

}

void GLCD_DrawRec (INT16U x, INT16U y, INT16U w, INT16U h, INT16U color)
{
	INT16U i;

	GLCD_SetWindow (x, y, w, h);
	wr_cmd(0x2c);
	
	for (i=0; i < (w*h); i++){
		wr_data16(color);
	}
	GLCD_WindowMax();

}

void GLCD_SetTextColor (unsigned short color) 
{
	TextColor = color;
}

unsigned short GLCD_GetTextColor (void) 
{
	return TextColor;
}

void GLCD_SetBackColor (unsigned short color) 
{
	BackColor = color;
}

unsigned short GLCD_GetBackColor (void) 
{
	return BackColor;
}

void GLCD_Clear (unsigned short color) 
{
	unsigned int i;
		
	wr_cmd(0x2A);
	wr_data(0);
	wr_data(0);;	

	wr_cmd(0x2B);
	wr_data(0);
	wr_data(0);

	wr_cmd(0x2c);
	
	for(i = 0; i < (WIDTH*HEIGHT); i++)
		wr_data16(color);
}

void GLCD_DrawChar_6x8(unsigned int x, unsigned int y, unsigned char bc, unsigned char c) 
{
	int idx = 0, i, j;
	if (0x1F < c && c < 0x90) {
		idx = (c - 0x20)*8;
		for (i = 0;i < 8;i++) {
			for (j = 0; j < 6; j++) {
				if (Font_6x8_h[idx+i] & 0x01<<(7-j)) {
					GLCD_DrawPixel (x+j, y+i, TextColor);
				}
				else {
					if (bc) {
						GLCD_DrawPixel (x+j, y+i, BackColor);
					}
				}
			}
		}
	}
}

void GLCD_DrawChar_8x12(unsigned int x, unsigned int y, unsigned char bc, unsigned char c) 
{
	int idx = 0, i, j;
	if (0x1F<c && c<0x90){
		idx = (c - 0x20)*12;
		for (i=0;i<12;i++){
			for (j=0;j<8;j++){
				if (Font_8x12_h[idx+i] & 0x01<<(7-j)){
					GLCD_DrawPixel (x+j, y+i, TextColor);

				}
				else{
					if (bc) {
						GLCD_DrawPixel (x+j, y+i, BackColor);
					}
				}
			}
		}
	}
}

void GLCD_DrawChar_8x16(unsigned int x, unsigned int y, unsigned char bc, unsigned char c) 
{
	int idx = 0, i, j;
	if (0x1F<c && c<0x90){
		idx = (c - 0x20)*16;
		for (i=0;i<16;i++) {
			for (j=0;j<8;j++){
				if (Font_8x16_h[idx+i] & 0x01<<(7-j)){
					GLCD_DrawPixel (x+j, y+i, TextColor);
				}
				else{
					if (bc) {
						GLCD_DrawPixel (x+j, y+i, BackColor);
					}
				}
			}
		}
	}
}

void GLCD_DrawChar_16x24(unsigned int x, unsigned int y, unsigned char bc, unsigned char c) 
{
	int idx = 0, i, j;
	if (0x1F<c && c<0x90) {
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

void GLCD_DisplayChar (unsigned int x, unsigned int y, unsigned char fi, unsigned char bc, char c) 
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

void GLCD_DisplayString	(unsigned int x, unsigned int y, unsigned char fi, unsigned char bc, char *s) 
{
	GLCD_WindowMax();
	
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

void GLCD_Bitmap (unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char *bitmap) 
{
	unsigned int i;
	unsigned short *bitmap_ptr = (unsigned short *)bitmap;
	GLCD_SetWindow (x, y, w, h);
 	Color_Dat_start();

	for(i = 0; i < (w*h); i++)
		wr_data16(*bitmap_ptr++);
	
	GLCD_WindowMax();
}
