/*
 *	brife: this is at88sc driver code,gpio code 
 */
 #include "s1.h"
 #include <rthw.h>
 #include <s3c44b0.h>
 /*config scl,sda as gpio mode and dir out*/
 #define DELAY 10
 void i2c_init(void)
{
	PCONE = PCONE & ~(0x3<<14);
	PCONE = PCONE | (0x1<<14);
	PCONF = PCONF & ~(0x7<<16);
	PCONF = PCONF | (0x1<<16);
	PUPF = PUPF & ~(0x1<<7);//scl
	PUPE = PUPE & ~(0x1<<7);//sda
}
/*scl set hi or low*/
void i2c_scl_set(unsigned char level)
{
    volatile unsigned long delay = 0;
    if(level)
        PDATF = PDATF | (0x1<<7);
    else
        PDATF = PDATF & ~(0x1<<7);
    for (delay = 0; delay < DELAY; )
    {
        delay++;
    }
}
/*sda set hi or low*/
void i2c_sda_set(unsigned char level)
{
    volatile unsigned long delay = 0;
    if(level)
        PDATE = PDATE | (0x1<<7);
    else
        PDATE = PDATE & ~(0x1<<7);
    for (delay = 0; delay < DELAY; )
    {
        delay++;
    }
}
/*return sda level */
unsigned char i2c_sda_get(void)
{
		return (PDATE & (0x1<<7))?1:0;
}
/*config sda dir input*/
void i2c_sda_input(void)
{
    volatile unsigned long delay = 0;
    PCONE = PCONE & ~(0x3<<14);
    for (delay = 0; delay < DELAY; )
    {
        delay++;
    }
}
/*config sda dir output*/
void i2c_sda_output(void)
{
	PCONE = PCONE & ~(0x3<<14);
	PCONE = PCONE | (0x1<<14);
}
/*sleep function*/
void sleep_ms(unsigned long n)
{
	volatile long i,j;
	for(i=0;i<n;i++)
	for(j=0;j<DELAY;j++)
	;
}
