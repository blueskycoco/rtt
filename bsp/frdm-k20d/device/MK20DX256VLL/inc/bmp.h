#ifndef __BMP_H_
#define __BMP_H_

#ifndef TRUE
#define TRUE		(unsigned char)(0x01)
#endif
#ifndef FALSE
#define FALSE		(unsigned char)(0x00)
#endif
#ifndef BOOL
#define BOOL		unsigned char
#endif
#ifndef INT8U
#define INT8U		unsigned char
#endif
#ifndef	INT16U
#define INT16U		unsigned short
#endif
#ifndef	INT32U
#define INT32U		unsigned long
#endif

#define DRAW_PIXEL			GLCD_DrawPixel		// interface DRAW_PIXEL(x16 ,y16 ,colr16)

typedef struct
{
	INT16U bfType;					// must be "BM"
	INT32U bfSize;					// the whole file size
	INT16U bfReservde1;				// defalut 0
	INT16U bfReservde2;				// defalut 0
	INT32U bfOffset;				// point to color data
}BmpFileHeader;

typedef struct
{
	INT32U biSize;
	INT32U biWidth;					// width of the Bmp
	INT32U biHeight;				// height of the Bmp
	INT16U biPlanes;
	INT16U biBitCount;
	INT32U biCompression;
	INT32U biSizeImage;
	INT32U biXPelsPerMeter;
	INT32U biYPelsPerMeter;
	INT32U biClrUsed;
	INT32U biClrImportant;
}BmpInfoHeader;

typedef struct
{
	INT8U rgbBlue;
	INT8U rgbGreen;
	INT8U rgbRed;
	INT8U rgbReserved;
}RgbQuad;

extern INT16U Rgb24to16(INT8U r,INT8U g,INT8U b);     // return RGB16 R5 G6 B5
extern BOOL CheckIsBmp(void *pBmp);
extern BOOL GetBmpHWSize(void *pBmp ,INT32U *WBmp ,INT32U *HBmp);
extern BOOL GetBmpFSize(void *pBmp ,INT32U *FSize);
extern BOOL GetColorOffser(void *pBmp ,INT32U *ColorOffset);
extern BOOL GetBmpInfo(void *pBmp ,BmpInfoHeader *BmpInfo);
extern BOOL DrawBmp(INT16U x ,INT16U y, void *pBmp);

#endif
