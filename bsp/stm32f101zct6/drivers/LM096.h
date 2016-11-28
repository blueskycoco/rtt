#ifndef __LM096_H
#define __LM096_H

void ssd1306_init(void);
int display(void);
void clear(void);
void draw1(uint8_t bat1,uint8_t bat2,char *c);
void draw(char *co2,char *co1);
#endif
