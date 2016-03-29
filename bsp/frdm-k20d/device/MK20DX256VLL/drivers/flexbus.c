#include "flexbus.h"

BOOL FLEXBUS_ClkEn (void)
{
	SIM->SCGC7 |= SIM_SCGC7_FLEXBUS_MASK;
	return (TRUE);
}

BOOL FLEXBUS_ClkDis (void)
{
	SIM->SCGC7 &= ~SIM_SCGC7_FLEXBUS_MASK;
	return (TRUE);
}

BOOL FLEXBUS_Init ( INT32U div_n )
{
	//fb clock divider 3
	SIM->CLKDIV1 |= SIM_CLKDIV1_OUTDIV3(div_n);		// div_n
	
	FB->CSPMCR = FB_CSPMCR_GROUP5(2)
				|FB_CSPMCR_GROUP4(1)
				|FB_CSPMCR_GROUP3(2)
				|FB_CSPMCR_GROUP2(2)
				|FB_CSPMCR_GROUP1(0)
				;
	
	//Set Base address
	FB->CS[2].CSAR = CS2_BASE;			// PSRAM

	FB->CS[2].CSCR = FB_CSCR_PS(3)      // 16-bit port
					|FB_CSCR_AA_MASK    // auto-acknowledge
//					|FB_CSCR_EXALE_MASK
 					|FB_CSCR_WRAH(5)//2
 					|FB_CSCR_RDAH(5)//2
// 				 	|FB_CSCR_SWSEN_MASK
// 				 	|FB_CSCR_SWS(2)
					|FB_CSCR_BLS_MASK
					|FB_CSCR_ASET(1)  // assert chip select on second clock edge after address is asserted
					|FB_CSCR_WS(1)    // 1 wait state - may need a wait state depending on the bus speed
					;
	
	FB->CS[2].CSMR = FB_CSMR_V_MASK    //Enable cs signal
					|FB_CSMR_BAM(0x007F);
//--------------------------------------------------------------------------------------------------------------
	//Set Base address
	FB->CS[0].CSAR = CS0_BASE;			// LCD

	FB->CS[0].CSCR = FB_CSCR_PS(3)      // 16-bit port	//ram
					|FB_CSCR_AA_MASK    // auto-acknowledge
					|FB_CSCR_BLS_MASK	// right aligned
					|FB_CSCR_BEM_MASK	// be signal is asserted for rw acess
					|FB_CSCR_ASET(1)    // assert chip select on second clock edge after address is asserted
					|FB_CSCR_EXTS_MASK
					|FB_CSCR_WRAH(0)
					|FB_CSCR_RDAH(0)
					|FB_CSCR_WS(1)      // 1 wait state - may need a wait state depending on the bus speed
					;

	FB->CS[0].CSMR = FB_CSMR_V_MASK;    //Enable cs signal
//--------------------------------------------------------------------------------------------------------------
	return (TRUE);
}

BOOL Mem_Rd ( INT16U *dat, INT32U offset )
{
	BOOL ret = TRUE;
	
	if ( offset < PSRAM_CY ) {
		*dat = *(INT16U*)(CS2_BASE + (offset<<1));
	}
	else {
		ret = FALSE;
	}
	
	return (ret);
}

BOOL Mem_Wr ( INT16U dat, INT32U offset )
{
	BOOL ret = TRUE;
		
	if ( offset < PSRAM_CY ) {
		*(INT16U*)(CS2_BASE + (offset<<1)) = dat;
	}
	else {
		ret = FALSE;
	}
	
	return (ret);
}
