/*
 * File      : led.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 *
 */

#include <MK20D7.h>
#include "led.h"

const rt_uint32_t led_mask[] = {1 << 12, 1 << 13, 1 << 24, 1 << 25};

void rt_hw_led_init(void)
{
    SIM->SCGC5 |= (1 << SIM_SCGC5_PORTC_SHIFT);
    SIM->SCGC5 |= (1 << SIM_SCGC5_PORTE_SHIFT);

    PORTC->PCR[12] &= ~PORT_PCR_MUX_MASK;
    PORTC->PCR[12] |= PORT_PCR_MUX(1);   //PTB12 is GPIO pin

    PORTC->PCR[13] &= ~PORT_PCR_MUX_MASK;
    PORTC->PCR[13] |= PORT_PCR_MUX(1);  //PTB13 is GPIO pin

    PORTE->PCR[24] &= ~PORT_PCR_MUX_MASK;
    PORTE->PCR[24] |= PORT_PCR_MUX(1);  //PTE24 is GPIO pin

    PORTE->PCR[25] &= ~PORT_PCR_MUX_MASK;
    PORTE->PCR[25] |= PORT_PCR_MUX(1);  //PTE25 is GPIO pin

    /* Switch LEDs off and enable output*/
	PTC->PDDR |= GPIO_PDDR_PDD(led_mask[1] | led_mask[0]);
	PTE->PDDR |= GPIO_PDDR_PDD(led_mask[3] |led_mask[2]);

    rt_hw_led_off(LED_RED);
    rt_hw_led_off(LED_GREEN);
    rt_hw_led_off(LED_BLUE);
    rt_hw_led_off(LED_MAX);
}

void rt_hw_led_uninit(void)
{
    PORTC->PCR[12] &= ~PORT_PCR_MUX_MASK;

    PORTC->PCR[13] &= ~PORT_PCR_MUX_MASK;

    PORTE->PCR[24] &= ~PORT_PCR_MUX_MASK;

    PORTE->PCR[25] &= ~PORT_PCR_MUX_MASK;
}

void rt_hw_led_on(rt_uint32_t n)
{
    	if(n==LED_BLUE||n==LED_RED)
        		PTC->PCOR |= led_mask[n];
	else
		PTE->PCOR |= led_mask[n];	
}

void rt_hw_led_off(rt_uint32_t n)
{
    	if(n==LED_BLUE||n==LED_RED)
        		PTC->PSOR |= led_mask[n];
	else
		PTE->PSOR |= led_mask[n];
}
/*pit trigger pdb , pdb trigger dac0 , dac0 trigger dma read from buffer*/
unsigned short buffer[16];

void dac_dma(void)
{

	int i;
	unsigned int g_bus_clock=(SystemCoreClock/(((SIM->CLKDIV1&SIM_CLKDIV1_OUTDIV2_MASK)>>SIM_CLKDIV1_OUTDIV2_SHIFT)+1));
	unsigned char pdb_sc_mults[4] = {1, 10, 20, 40};
	unsigned int  us = 5*1000;
  	unsigned int  delayus = 0,mod;
	unsigned int bus_clk = g_bus_clock/1000000;
  	unsigned char prescaler, mult,j;
	float temp=0;
	unsigned int ldval = 500*1000*(g_bus_clock/1000000);

	rt_kprintf("g_bus_clock is %d\r\n",g_bus_clock);
	for(i=0;i<16;i++)
		buffer[i]=1000+i*100;
	/*1 config dac0*/
	SIM->SCGC2 |= SIM_SCGC2_DAC0_MASK;
	for(i=0;i<16;i++)
	{
		DAC0->DAT[i].DATL=0;
  		DAC0->DAT[i].DATH=0;
	}
	DAC0->C2 = DAC_C2_DACBFUP(0x0f);
	DAC0->C1 = 0;
	DAC0->C1 |= DAC_C1_DACBFMD(0x00);
	DAC0->C1 |= DAC_C1_DACBFWM(0x03);
	DAC0->C1 |= DAC_C1_DMAEN_MASK; 
	DAC0->C1 |= DAC_C1_DACBFEN_MASK; 
	DAC0->C0 = 0;
	DAC0->C0 |= DAC_C0_DACBBIEN_MASK|DAC_C0_DACBTIEN_MASK|DAC_C0_DACBWIEN_MASK/*|DAC_C0_DACTRGSEL_MASK*/;
	DAC0->C0 |= DAC_C0_DACRFS_MASK;
	DAC0->C0 |= DAC_C0_DACEN_MASK;
	/*2 config dma0*/
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
	DMA0->ERQ &= ~(1<<0);
	DMAMUX->CHCFG[0] = DMAMUX_CHCFG_SOURCE(45);
	DMAMUX->CHCFG[0] &= ~(DMAMUX_CHCFG_TRIG_MASK);
	DMA0->EEI|=DMA_EEI_EEI0_MASK;
	DMA0->TCD[0].SADDR = DMA_SADDR_SADDR(buffer);
	DMA0->TCD[0].SOFF = DMA_SOFF_SOFF(2);
	DMA0->TCD[0].ATTR = 0 | DMA_ATTR_SSIZE(1);
	DMA0->TCD[0].SLAST = DMA_SLAST_SLAST(-32);
	DMA0->TCD[0].DADDR = DMA_DADDR_DADDR((unsigned int)&(DAC0->DAT[0].DATL));
	DMA0->TCD[0].DOFF = DMA_DOFF_DOFF(2);
	DMA0->TCD[0].ATTR |= DMA_ATTR_DSIZE(1);
	DMA0->TCD[0].DLAST_SGA = DMA_DLAST_SGA_DLASTSGA(-32);
	DMA0->TCD[0].CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(1);//?
	DMA0->TCD[0].BITER_ELINKNO = DMA_CITER_ELINKNO_CITER(1);//?
	DMA0->TCD[0].NBYTES_MLNO = DMA_NBYTES_MLNO_NBYTES(32);//?
	DMA0->TCD[0].CSR = 0;
	DMA0->TCD[0].CSR &= ~(DMA_CSR_DREQ_MASK);
	DMAMUX->CHCFG[0] |= DMAMUX_CHCFG_ENBL_MASK;
	DMA0->ERQ|=(1<<0);
	/*3 config pdb0*/	
	for(i=0; i<4; i++)
	{
		mult = i;
		for(j=0; j<8; j++)
		{
			prescaler = j;
			mod = (bus_clk*us)/((1<<j)*pdb_sc_mults[i]);
			if(mod <= 0xFFFFu)
				break;
		}
		if(mod <= 0xFFFFu)
			break;
		else if(i == 3)
		{
			rt_kprintf("return %d\r\n",i);
			return;         
		}
	}
	rt_kprintf("mod %d , mult %d , prescaler %d , us %d , delayus %d , temp %l busclk %d\r\n",mod,mult,prescaler,us,delayus,temp,bus_clk);
	SIM->SCGC6 |= SIM_SCGC6_PDB_MASK;
	PDB0->SC = 0x00;
	PDB0->SC |= PDB_SC_PDBEN_MASK|PDB_SC_PDBIE_MASK|PDB_SC_PDBEIE_MASK;
	PDB0->SC |= PDB_SC_MULT(mult);
	PDB0->SC |= PDB_SC_PRESCALER(prescaler);
	PDB0->SC |= PDB_SC_LDMOD(0); 
	PDB0->SC |= PDB_SC_TRGSEL(4);//point to pit0
	PDB0->SC &= ~(PDB_SC_CONT_MASK);
	PDB0->SC &= ~(PDB_SC_DMAEN_MASK);
	if( delayus <= us )
  	{
	    	temp = (float)mod/(float)us;
	    	PDB0->IDLY = (unsigned int)(delayus*temp);
  	}
	//PDB0->MOD = PDB_MOD_MOD(400);
	//PDB0->SC |= PDB_SC_LDOK_MASK;
	PDB0->DAC[0].INTC=0;
	PDB0->DAC[0].INTC |= PDB_INTC_TOE_MASK;	//point to dac0
	PDB0->DAC[0].INT = (bus_clk * 500)/(prescaler * mult);
	PDB0->SC |= PDB_SC_LDOK_MASK;	
	rt_kprintf("DAC[0].INT %d PDB0->IDLY %d\r\n",(bus_clk * 500)/(prescaler * mult),PDB0->IDLY);
	/*4 config pit0*/	
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;
	PIT->MCR |= PIT_MCR_FRZ_MASK;
	PIT->CHANNEL[0].LDVAL = ldval-1;
	PIT->CHANNEL[0].TFLG  |= PIT_TFLG_TIF_MASK;
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK|PIT_TCTRL_TIE_MASK;//open pit intr
	NVIC_EnableIRQ(PIT0_IRQn);
	NVIC_EnableIRQ(PDB0_IRQn);
	NVIC_EnableIRQ(DMA0_IRQn);
	NVIC_EnableIRQ(DMA_Error_IRQn);
	NVIC_EnableIRQ(DAC0_IRQn);
}
int j=0;
void PIT0_IRQHandler(void)
{
	int i;
	rt_interrupt_enter();

	PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
	//rt_kprintf("pit0 intr %d\r\n",PDB0->CNT&0xffff);
	
	rt_kprintf("\r\n");
	for(i=0;i<16;i++)
		rt_kprintf("dac_buf[%2d] %x %x\r\n",i,DAC0->DAT[i].DATL,DAC0->DAT[i].DATH);
	//DMA0->TCD[0].CSR |= DMA_CSR_START_MASK;
	//DMA0->ERQ|=(1<<0);
	for(i=0;i<16;i++)
		buffer[i]=2000+j+i*100;
	j++;
	rt_interrupt_leave();
}

void PDB0_IRQHandler(void)
{
	rt_interrupt_enter();

	if((PDB0->SC & PDB_SC_PDBIF_MASK) && (PDB0->SC & PDB_SC_PDBIE_MASK))
	{
		PDB0->SC &= ~PDB_SC_PDBIF_MASK;
	}


	if((PDB0->CH[0].S & PDB_S_ERR(1)) && (PDB0->SC & PDB_SC_PDBEIE_MASK))
	{
		//rt_kprintf("pdb0 err intr \r\n");
		PDB0->CH[0].S |= PDB_S_ERR(1);
	}

	rt_interrupt_leave();
}
void DAC0_IRQHandler(void)
{
	//int i;
	rt_interrupt_enter();

  if((DAC0->SR & DAC_SR_DACBFRPBF_MASK) && (DAC0->C0 & DAC_C0_DACBBIEN_MASK))
  {
    rt_kprintf("dac0 DACBFRPBF intr \r\n");
    DAC0->SR &= ~(DAC_SR_DACBFRPBF_MASK);
  }
  if((DAC0->SR & DAC_SR_DACBFRPTF_MASK) && (DAC0->C0 & DAC_C0_DACBTIEN_MASK))
  {
    rt_kprintf("dac0 DACBFRPTF intr \r\n");
    DAC0->SR &= ~(DAC_SR_DACBFRPTF_MASK);
  }
  if((DAC0->SR & DAC_SR_DACBFWMF_MASK) && (DAC0->C0 & DAC_C0_DACBWIEN_MASK))
  {
    rt_kprintf("dac0 DACBFWMF intr \r\n");
    DAC0->SR &= ~(DAC_SR_DACBFWMF_MASK);
  }
  
  rt_interrupt_leave();
}
void DMA0_isr(void)
{
	//DMAMUX->CHCFG[0] = 0;
	rt_interrupt_enter();
	DMA0->INT =0x01;
	DMA0->CDNE = 0x01;
	rt_kprintf("dma0 intr \r\n");
	rt_interrupt_leave();
}
void DMA_Error_IRQHandler()
{
	rt_interrupt_enter();
	//DMA0->INT =0x01;
	//DMA0->CDNE = 0x01;
	rt_kprintf("DMA_Error_IRQHandler intr \r\n");
	rt_interrupt_leave();
}

