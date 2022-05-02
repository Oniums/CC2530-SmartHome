#ifndef __COORIO_H__
#define __COORIO_H__

void LIGHT_ON(void);
void LIGHT_OFF(void);
unsigned int LIGHT_STATE(void);

void Delay_us(void);
void Delay_10us(void);
void Delay_ms(unsigned int Time);
#endif