/**************************************************************************************************
*			file: gpio.h
*			
*			date: 2013.2.28 Manley
**************************************************************************************************/
#ifndef	__GPIO_H
#define	__GPIO_H
///////////////////////////////////////////////////////////
#include "kinetis.h"
#include "type.h"
///////////////////////////////////////////////////////////
#define IN 		((INT32U)0x00000000)
#define OUT 	((INT32U)0x00000001)
///////////////////////////////////////////////////////////
#define IO_0 	((INT32U)0x00000001)
#define	IO_1 	((INT32U)0x00000002)
#define	IO_2 	((INT32U)0x00000004)
#define	IO_3 	((INT32U)0x00000008)
#define	IO_4 	((INT32U)0x00000010)
#define	IO_5 	((INT32U)0x00000020)
#define	IO_6 	((INT32U)0x00000040)
#define	IO_7 	((INT32U)0x00000080)
#define	IO_8 	((INT32U)0x00000100)
#define	IO_9 	((INT32U)0x00000200)
#define	IO_10 	((INT32U)0x00000400)
#define	IO_11 	((INT32U)0x00000800)
#define	IO_12 	((INT32U)0x00001000)
#define	IO_13 	((INT32U)0x00002000)
#define	IO_14 	((INT32U)0x00004000)
#define	IO_15 	((INT32U)0x00008000)
#define	IO_16 	((INT32U)0x00010000)
#define	IO_17 	((INT32U)0x00020000)
#define	IO_18 	((INT32U)0x00040000)
#define	IO_19 	((INT32U)0x00080000)
#define	IO_20 	((INT32U)0x00100000)
#define	IO_21 	((INT32U)0x00200000)
#define	IO_22 	((INT32U)0x00400000)
#define	IO_23 	((INT32U)0x00800000)
#define	IO_24 	((INT32U)0x01000000)
#define	IO_25 	((INT32U)0x02000000)
#define	IO_26 	((INT32U)0x04000000)
#define	IO_27 	((INT32U)0x08000000)
#define	IO_28 	((INT32U)0x10000000)
#define	IO_29 	((INT32U)0x20000000)
#define	IO_30 	((INT32U)0x40000000)
#define	IO_31 	((INT32U)0x80000000)

static __inline void GPIO_BitDir ( GPIO_Type *PTx, INT32U bit_n, INT32U bit_dir)
{
	(bit_dir == 0) ? (PTx->PDDR &= ~bit_n) : (PTx->PDDR |= bit_n);
}
static __inline void GPIO_PortDir ( GPIO_Type *PTx, INT32U port_dir)
{
	PTx->PDDR = port_dir;
}
static __inline void GPIO_SetBit ( GPIO_Type *PTx, INT32U bit_n)
{
	PTx->PSOR = bit_n;
}
static __inline void GPIO_ClrBit ( GPIO_Type *PTx, INT32U bit_n)
{
	PTx->PCOR = bit_n;
}
static __inline INT32U GPIO_RdPort ( GPIO_Type *PTx)
{
	return (PTx->PDIR);
}
static __inline INT32U GPIO_RdBit ( GPIO_Type *PTx, INT32U bit_n)
{
	INT32U ret;	
	ret = ((PTx->PDIR & bit_n) == 0) ? (INT32U)0x0 : (INT32U)0x1;
	return (ret);
}
static __inline void GPIO_WrPort ( GPIO_Type *PTx, INT32U val)
{
	PTx->PDOR = val;
}
static __inline void GPIO_WrBit ( GPIO_Type *PTx, INT32U bit_n, INT32U val)
{
	(val == 0) ? (PTx->PDOR &= bit_n) : (PTx->PDOR |= bit_n);
}
static __inline void GPIO_TogglePort ( GPIO_Type *PTx)
{
	PTx->PTOR = (INT32U)0xFFFFFF;
}
static __inline void GPIO_ToggleBit ( GPIO_Type *PTx, INT32U bit_n)
{
	PTx->PTOR |= bit_n;
}

#endif
