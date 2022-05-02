#include <ioCC2530.h>
#include "OnBoard.h"
#include "dht11.h"
#define FUN1 P1_2
#define KEY1 P0_1

void SENSIO_Init(void);
__interrupt void S1_ISR(void);
void OPEN_FUN1(void);
void CLOSE_FUN1(void);
void FUNS_STATE(unsigned int *FUN1_S);

void SENSIO_Init(void)
{
    P1DIR |= 0X0C;
    FUN1 = 1;

    P0INP |= 0x02; //上拉
    P0IEN |= 0X02; //P01设置为中断方式
    PICTL |= 0X01; //下降沿触发
    EA = 1;
    IEN1 |= 0X20;  // P0设置为中断方式;
    P0IFG |= 0x00; //初始化中断标志位
}

#pragma vector = P0INT_VECTOR
__interrupt void S1_ISR(void)
{
    Delay_ms(500);
    FUN1 = !FUN1;
    P0IFG = 0;
    P0IF = 0;
}

void OPEN_FUN1(void)
{
    FUN1 = 0;
}
void CLOSE_FUN1(void)
{
    FUN1 = 1;
}
void FUNS_STATE(unsigned int *FUN1_S)
{
    *FUN1_S = !FUN1;
}
