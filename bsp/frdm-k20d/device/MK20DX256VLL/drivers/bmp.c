#include "string.h"
#include "bmp.h"
 
/*-------------------------------------------------------------------------
|	INT16U Rgb24to16(INT8U r,INT8U g,INT8U b)
|
|
|
|-------------------------------------------------------------------------*/
INT16U Rgb24to16(INT8U r,INT8U g,INT8U b)
{
	INT16U rgb16=0;					// RGB  565
	
	r &= 0xF8;
	rgb16 |= (INT16U)r;
	rgb16 <<= 5;
	
	g &= 0xFC;
	rgb16 |= (INT16U)g;
	rgb16 <<= 3;

	b >>= 3;
	rgb16 |= (INT16U)b;
	return rgb16;
}
/*-------------------------------------------------------------------------
|	INT8U CheckIsBmp(void *pBmp)
|
|
|
|-------------------------------------------------------------------------*/
BOOL CheckIsBmp(void *pBmp)
{
	if(pBmp==0)
	{
		return FALSE;
	}
	if((*(INT8U *)pBmp == 'B') && (*((INT8U *)pBmp + 1) == 'M'))  // if the first two byte is ascll "BM" then that is a Bmp file
	{	
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
/*-------------------------------------------------------------------------
|	BOOL GetBmpFSize(void *pBmp ,INT32U FSize)
|
|
|
|-------------------------------------------------------------------------*/
BOOL GetBmpFSize(void *pBmp ,INT32U *FSize)
{
	if(FSize==0)
	{
		return FALSE;
	}
	if(!CheckIsBmp(pBmp))
	{
		return FALSE;
	}

	*FSize = *(INT32U *)((INT8U *)pBmp + 2);

	return TRUE;
}
/*-------------------------------------------------------------------------
|	BOOL GetBmpHWSize(void *pBmp ,INT32U WBmp ,INT32U HBmp)
|
|
|
|-------------------------------------------------------------------------*/
BOOL GetBmpHWSize(void *pBmp ,INT32U *WBmp ,INT32U *HBmp)
{
	if(WBmp==0 || HBmp==0)
	{
		return FALSE;
	}
	if(!CheckIsBmp(pBmp))
	{
		return FALSE;
	}

	memcpy(WBmp ,(INT8U *)pBmp+18 ,4);
	memcpy(HBmp ,(INT8U *)pBmp+22 ,4);

	return TRUE;
}
/*-------------------------------------------------------------------------
|	BOOL GetColorOffser(void *pBmp ,INT32U ColorOffset)
|
|
|
|-------------------------------------------------------------------------*/
BOOL GetColorOffser(void *pBmp ,INT32U *ColorOffset)
{
	if(ColorOffset==0)
	{
		return FALSE;
	}
	if(!CheckIsBmp(pBmp))
	{
		return FALSE;
	}
	memcpy(ColorOffset ,(INT8U *)pBmp+10 ,4);

	return TRUE;
}
/*-------------------------------------------------------------------------
|	BOOL GetBmpInfo(void *pBmp ,BmpInfoHeader *BmpInfo)
|
|
|
|-------------------------------------------------------------------------*/
BOOL GetBmpInfo(void *pBmp ,BmpInfoHeader *BmpInfo)
{
	if(BmpInfo==0)
	{
		return FALSE;
	}
	if(!CheckIsBmp(pBmp))
	{
		return FALSE;
	}
	
	memcpy(BmpInfo ,(INT8U *)pBmp+14 ,sizeof(BmpInfoHeader));
	
	return TRUE;		
}
/*-------------------------------------------------------------------------
|	BOOL DrawBmp(void *pBmp ,)
|
|
|
|-------------------------------------------------------------------------*/
BOOL DrawBmp(INT16U x ,INT16U y, void *pBmp)
{
	INT32U w_temp,WBmp;
	INT32U h_temp,HBmp;
	INT32U ColorOffset;
	INT16U color_temp;
	INT32U t;
	INT8U  *offset_temp;

	if(!GetBmpHWSize(pBmp ,&WBmp ,&HBmp)) {
		return FALSE;
	}
	
	if(!GetColorOffser(pBmp ,&ColorOffset)) {
		return FALSE;
	}
	
	t = 4-(WBmp*3)%4;													//每行补齐字节数   每行数据为4的整数倍
	if (4==t) {t = 0;}
	
	for (h_temp = 0 ;h_temp < HBmp ; h_temp++) {
		offset_temp = (INT8U *)pBmp + ColorOffset + (HBmp-h_temp-1)*(3*WBmp+t);

		for (w_temp = 0 ;w_temp < WBmp ; w_temp++) {	
			color_temp = (INT16U)Rgb24to16(*(offset_temp+w_temp*3+2) ,*(offset_temp+w_temp*3+1) ,*(offset_temp+w_temp*3));
			DRAW_PIXEL(x+w_temp ,y+h_temp , color_temp);
		}
	}
	
	return TRUE;
}
