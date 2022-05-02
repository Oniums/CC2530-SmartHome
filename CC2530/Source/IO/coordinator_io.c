#include <ioCC2530.h>
#include "OnBoard.h"

#define LED P1_1

void SENSIO_Init(void);
void LIGHT_ON(void);
void LIGHT_OFF(void);
unsigned int LIGHT_STATE(void);

void Delay_us(void);
void Delay_10us(void);
void Delay_ms(unsigned int Time);
void LIGHT_ON(void)
{
    LED = 1;
}
void LIGHT_OFF(void)
{
    LED = 0;
}
unsigned int LIGHT_STATE(void)
{
    return !LED;
}

void Delay_us(void) //1 us延时
{
    MicroWait(1);
}

void Delay_10us(void) //10 us延时
{
    MicroWait(12);
}

void Delay_ms(unsigned int Time) //n ms延时
{
    unsigned char i;
    while (Time--)
    {
        for (i = 0; i < 100; i++)
            Delay_10us();
    }
}