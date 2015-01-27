/**************************************************************************************************
*			file: gpio.c
*			
*			date: 2013.2.28 Manley
*7.30
**************************************************************************************************/
#include "gpio.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
////设置端口n的第n位的方向（输入或输出）
//BOOL GPIO_BitDir ( GPIO_Type *PTx, INT32U bit_n, INT32U bit_dir)
//{
//	(bit_dir == 0) ? (PTx->PDDR &= ~bit_n) : (PTx->PDDR |= bit_n);
//	return (TRUE);
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////
////设置端口n全部位的方向（输入或输出）
//BOOL GPIO_PortDir ( GPIO_Type *PTx, INT32U port_dir)
//{
//	PTx->PDDR = port_dir;
//	return (TRUE);
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////
////端口n的第n位输出1
//BOOL GPIO_SetBit ( GPIO_Type *PTx, INT32U bit_n)
//{
//	PTx->PSOR = bit_n;
//	return (TRUE);
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////
////端口n的第n位输出0
//BOOL GPIO_ClrBit ( GPIO_Type *PTx, INT32U bit_n)
//{
//	PTx->PCOR = bit_n;
//	return (TRUE);
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////
////读端口n的所有位
//INT32U GPIO_RdPort ( GPIO_Type *PTx)
//{
//	return (PTx->PDIR);
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////
////读端口n的第n位
//INT32U GPIO_RdBit ( GPIO_Type *PTx, INT32U bit_n)
//{
//	INT32U ret;
//	
//	ret = ((PTx->PDIR & bit_n) == 0) ? (INT32U)0x0 : (INT32U)0x1;
//	return (ret);
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////
////向端口n写所有位
//BOOL GPIO_WrPort ( GPIO_Type *PTx, INT32U val)
//{
//	PTx->PDOR = val;
//	return (TRUE);
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////
////向端口n的第n位写数据（0或1）
//BOOL GPIO_WrBit ( GPIO_Type *PTx, INT32U bit_n, INT32U val)
//{
//	(val == 0) ? (PTx->PDOR &= bit_n) : (PTx->PDOR |= bit_n);
//	return (TRUE);
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////
////端口n的所有位翻转输出
//BOOL GPIO_TogglePort ( GPIO_Type *PTx)
//{
//	PTx->PTOR = (INT32U)0xFFFFFF;
//	return (TRUE);
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////
////端口n的第n位翻转输出
//BOOL GPIO_ToggleBit ( GPIO_Type *PTx, INT32U bit_n)
//{
//	PTx->PTOR |= bit_n;
//	return (TRUE);
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////
