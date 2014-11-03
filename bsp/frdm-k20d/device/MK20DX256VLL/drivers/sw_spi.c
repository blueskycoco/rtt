#include "sw_spi.h"
///////////////////////////////////////////////////////////////////////////////////////////////////

#define SW_SPI_CLK_H()		GPIO_SetBit(PTB, IO_21)
#define SW_SPI_CLK_L()		GPIO_ClrBit(PTB, IO_21)
#define GET_SW_SPI_DI()		GPIO_RdBit(PTB, IO_23)
#define SW_SPI_DO_H()		GPIO_SetBit(PTB, IO_22)
#define SW_SPI_DO_L()		GPIO_ClrBit(PTB, IO_22)
#define SW_SPI_CS0_H()		GPIO_SetBit(PTD, IO_7)
#define SW_SPI_CS0_L()		GPIO_ClrBit(PTD, IO_7)
#define SW_SPI_CS1_H()		GPIO_SetBit(PTB, IO_20)
#define SW_SPI_CS1_L()		GPIO_ClrBit(PTB, IO_20)

void SW_SPI_IO_INIT()
{
	PORT_ClkEn(PORTB);
	PORT_BitFn(PORTB, PIN_21, FN_1);	// clk
	PORT_BitFn(PORTB, PIN_22, FN_1);	// mosi
	PORT_BitFn(PORTB, PIN_23, FN_1);	// miso
	
	GPIO_BitDir(PTB, IO_21, OUT);
	GPIO_BitDir(PTB, IO_22, OUT);
	GPIO_BitDir(PTB, IO_23, IN);
	
	PORT_BitFn(PORTB, PIN_20, FN_1);	// cs1
	GPIO_SetBit(PTB, IO_20);
	GPIO_BitDir(PTB, IO_20, OUT);
	
	PORT_ClkEn(PORTD);
	PORT_BitFn(PORTD, PIN_7, FN_1);	// cs0
	GPIO_SetBit(PTD, IO_7);
	GPIO_BitDir(PTD, IO_7, OUT);
}


///////////////////////////////////////////////////////////////////////////////////////////////////

SW_SPI_mode_type SW_arg;

// 4种模式总线时序，操作序列。请勿修改
const SW_SPI_seq_type SW_seq[4] =
{
	{0x10, 0x29, 0x12, 0x07},		// cpha:0 cpol:0
	{0x18, 0x21, 0x1A, 0x0E},		// cpha:0 cpol:1
	{0x00, 0x19, 0x22, 0x07},		// cpha:1 cpol:0
	{0x08, 0x11, 0x2A, 0x0E}		// cpha:1 cpol:1
};

void SW_SPI_dly(void)
{
	unsigned char tmp = SW_arg.dly;
	while(tmp--);
}

void SW_SPI_Init(unsigned char sw_dly, unsigned char sw_bits, unsigned char sw_lsbe, unsigned char sw_cpol, unsigned char sw_cpha)
{
	SW_arg.dly  = sw_dly;
	SW_arg.bits = sw_bits;
	SW_arg.lsbe = sw_lsbe;
	SW_arg.cpol = sw_cpol;
	SW_arg.cpha = sw_cpha;
	
	// 8 bits
	if ((0<sw_bits)&&(sw_bits<=8)) {
		SW_arg.type = 1;
	}
	// 16 bits
	else if ((8<sw_bits)&&(sw_bits<=16)) {
		SW_arg.type = 2;
	}
	// 32 bits
	else {
		SW_arg.type = 4;
	}
	
	SW_SPI_IO_INIT();
	SW_SPI_DO_H();
	
	if (SW_arg.cpol == 0) {
		SW_SPI_CLK_L();
	}
	else {
		SW_SPI_CLK_H();
	}
}

unsigned char SW_SPI_shift(unsigned int data, unsigned char bits, unsigned char sr, unsigned char sn)
{
	unsigned int mask;
	unsigned char ret;
	
	if (sr==0) {
		sn = bits -1 - sn;
	}
	
	mask = 1<<sn;
	ret = ((data&mask)==0) ? 0 : 1;
	return (ret);
}

unsigned int SW_SPI_act(unsigned char mode, unsigned char seq, unsigned char val)
{
	unsigned int lvl_di = SW_SPI_XX;
	
	if (SW_seq[mode].s[seq].clk == 0) {
		SW_SPI_CLK_L();
	}
	else {
		SW_SPI_CLK_H();
	}
	
	switch (val) {
		case 0:
			SW_SPI_DO_L();
			break;
		case 1:
			SW_SPI_DO_H();
			break;
		case SW_SPI_RD:
			lvl_di = GET_SW_SPI_DI();
			break;
		case SW_SPI_XX:
			break;
		default:
			break;
	}
		
	return (lvl_di);
}

unsigned int SW_SPI_RxTx(unsigned int tx_data)
{
	unsigned int di = 0;
	unsigned char i = 0;
	unsigned char j = 0;
	unsigned char t = 0;
	unsigned char mode = 0;
	
	mode = SW_arg.cpha*2 + SW_arg.cpol;
	
	/* step 1 */
	if (SW_seq[mode].s[i].dat==1) {
		SW_SPI_act(mode, SW_SPI_SEG_H, SW_SPI_shift(tx_data, SW_arg.bits, SW_arg.lsbe, i));
		t++;
	}
	else {
		SW_SPI_act(mode, i, SW_SPI_XX);
	}
	SW_SPI_dly();
	/* step 2 reapet n times (n=bits) */
	for (j=0; j<SW_arg.bits; j++) {
		
		for (i=SW_SPI_SEG_S; i<SW_SPI_SEG_R; i++) {
			if (SW_seq[mode].s[i].dat != 0) {
				if ((j+t)< SW_arg.bits) {
					SW_SPI_act(mode, i, SW_SPI_shift(tx_data, SW_arg.bits, SW_arg.lsbe, j+t));
				}
				else {
					SW_SPI_act(mode, i, 1);
				}
			}
			else if (SW_seq[mode].s[i].latch != 0){
				if (SW_arg.lsbe!=0) {
					di |= SW_SPI_act(mode, i, SW_SPI_RD) << j;
				}
				else {
					di |= SW_SPI_act(mode, i, SW_SPI_RD) << (SW_arg.bits-1-j);
				}
			}
			else {
				;
			}
			SW_SPI_dly();			
		}
	}
	/* step 3 */	
	SW_SPI_act(mode, i, SW_SPI_XX);
	SW_SPI_dly();
	
	return (di);
}

void SW_SPI_Tx(void *tx_buff, unsigned int tx_len)
{
	unsigned int offset = 0;
	
	switch (SW_arg.type) {
		case 1:
			while(tx_len-- != 0) {
				SW_SPI_RxTx(*((unsigned char*)tx_buff + offset));
				offset++;
			}
			break;
		case 2:
			while(tx_len-- != 0) {
				SW_SPI_RxTx(*((unsigned short*)tx_buff + offset));
				offset++;
			}
			break;
		case 4:
			while(tx_len-- != 0) {
				SW_SPI_RxTx(*((unsigned int*)tx_buff + offset));
				offset++;
			}
			break;
		default:
			break;
	}
}

void SW_SPI_Rx(void *rx_buff, unsigned int rx_len)
{
	unsigned int offset = 0;
	
	switch (SW_arg.type) {
		case 1:
			while(rx_len-- != 0) {
				*((unsigned char*)rx_buff + offset) = SW_SPI_RxTx(0x0);
				offset++;
			}
			break;
		case 2:
			while(rx_len-- != 0) {
				*((unsigned short*)rx_buff + offset) = SW_SPI_RxTx(0x0);
				offset++;
			}
			break;
		case 4:
			while(rx_len-- != 0) {
				*((unsigned int*)rx_buff + offset) = SW_SPI_RxTx(0x0);
				offset++;
			}
			break;
		default:
			break;
	}
}
