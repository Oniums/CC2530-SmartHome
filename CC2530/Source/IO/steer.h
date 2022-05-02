#ifndef __STEER_H__
#define __STEER_H__


void STEER_INIT(void);
void servo(int angle);
void open(void);
void close(void);
void Delay_ms(unsigned int Time);
void Delay_10us(void);
void Delay_us(void);
unsigned int STATE(void);
void warming(void);
void no_warm(void);
#endif