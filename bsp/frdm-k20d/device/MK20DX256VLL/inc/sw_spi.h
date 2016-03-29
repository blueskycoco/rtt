#ifndef __SW_SPI_H
#define __SW_SPI_H

#include "port.h"
#include "gpio.h"

///////////////////////////////////////////////////////////////////////////////
#define SW_SPI_NUM			((unsigned char)0x03)		// 软件spi数量
///////////////////////////////////////////////////////////////////////////////
#define SW_SPI_LSBE			((unsigned char)0x01)		// 低位先移出
#define SW_SPI_MSBE			((unsigned char)0x00)		// 高位先移出

#define SW_SPI_POL1			((unsigned char)0x01)		// clk 空闲高电平
#define SW_SPI_POL0			((unsigned char)0x00)		// clk 空闲低电平

#define SW_SPI_PHA0			((unsigned char)0x00)		// 第一个沿采样
#define SW_SPI_PHA1			((unsigned char)0x01)		// 第二个沿采样


///////////////////////////////////////////////////////////////////////////////
#define SW_SPI_SEG_H		((unsigned char)0x00)
#define SW_SPI_SEG_S		((unsigned char)0x01)
#define SW_SPI_SEG_E		((unsigned char)0x02)
#define SW_SPI_SEG_R		((unsigned char)0x03)

#define SW_SPI_RD			((unsigned char)0x02)
#define SW_SPI_XX			((unsigned char)0x03)

#define SW_SPI_CS0			((unsigned char)0x00)
#define SW_SPI_CS1			((unsigned char)0x01)
#define SW_SPI_CS2			((unsigned char)0x02)

typedef struct
{
	unsigned char lsbe		: 1;	// 小端
	unsigned char cpol		: 1;	// 极性
	unsigned char cpha		: 1;	// 相位
	unsigned char cont		: 1;	// 片选连续
	unsigned char busy		: 1;	// 忙标志
	unsigned char bits;				// 位数
	unsigned char dly;				// 延时
	unsigned char type;				// 类型
}SW_SPI_mode_type;

typedef struct
{
	unsigned char ctrl		: 2;	// 0:normal  1:repeat  2:repeat end  3:frame end
	unsigned char cs		: 1;
	unsigned char clk		: 1;
	unsigned char dat		: 1;
	unsigned char latch		: 1;
	unsigned char reserve	: 2;
}SW_SPI_sample_type;

typedef union
{
	unsigned char Bs[4];
	SW_SPI_sample_type s[4];
}SW_SPI_seq_type;



///////////////////////////////////////////////////////////////////////////////
void SW_SPI_Init(unsigned char sw_dly, unsigned char sw_bits, unsigned char sw_lsbe, unsigned char sw_cpol, unsigned char sw_cpha);
void SW_SPI_Tx(void *tx_buff, unsigned int tx_len);
void SW_SPI_Rx(void *rx_buff, unsigned int rx_len);
unsigned int SW_SPI_RxTx(unsigned int tx_data);

#endif
