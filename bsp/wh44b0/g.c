/*
 *	brife: this is at88sc driver code,gpio code 
 */
 #include "s1.h"
 #include <rthw.h>
 #include <s3c44b0.h>
 /*config scl,sda as gpio mode and dir out*/
 void i2c_init(void)
{
    PCONE = PCONE & ~(0x3<<6);
	PCONE = PCONE | (0x1<<6);
	PCONE = PCONE & ~(0x3<<8);
	PCONE = PCONE | (0x1<<8);
	PUPE = PUPE | (0x1<<3);//scl
	PUPE = PUPE | (0x1<<4);//sda
}
/*scl set hi or low*/
void i2c_scl_set(unsigned char level)
{
    volatile unsigned long delay = 0;
    if(level)
        PDATE = PDATE | (0x1<<3);
    else
        PDATE = PDATE & ~(0x1<<3);
    for (delay = 0; delay < 100; )
    {
        delay++;
    }
}
/*sda set hi or low*/
void i2c_sda_set(unsigned char level)
{
    volatile unsigned long delay = 0;
    if(level)
        PDATE = PDATE | (0x1<<4);
    else
        PDATE = PDATE & ~(0x1<<4);
    for (delay = 0; delay < 100; )
    {
        delay++;
    }
}
/*return sda level */
unsigned char i2c_sda_get(void)
{
    return (PDATE & (0x1<<4))?1:0;
}
/*config sda dir input*/
void i2c_sda_input(void)
{
    volatile unsigned long delay = 0;
    PCONE = PCONE & ~(0x3<<8);
    for (delay = 0; delay < 100; )
    {
        delay++;
    }
}
/*config sda dir output*/
void i2c_sda_output(void)
{
    PCONE = PCONE | (0x1<<8);
}
/*sleep function*/
void sleep_ms(unsigned long n)
{
    rt_thread_delay(n/10);
}

 
 