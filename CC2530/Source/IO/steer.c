#include <ioCC2530.h>
#include "OnBoard.h"
#define BEEP P1_2

unsigned int STEER_STATE = 0;

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
__interrupt void S1_ISR(void);
void STEER_INIT(void)
{
    CLKCONCMD &= ~0x40; //设置系统时钟源为32MHZ晶振
    while (CLKCONSTA & 0x40)
        ;               //等待晶振稳定为32M
    CLKCONCMD &= ~0x07; //设置系统主时钟频率为32MHZ
    CLKCONCMD |= 0x38;  //时钟速度32 MHz 定时器标记输出设置[5:3]250kHz

    PERCFG |= 0x40; //定时器1 的IO位置   1:备用位置2
    P2SEL &= ~0x10; //定时器1优先
    P2DIR |= 0xC0;  //第1优先级：定时器1通道2-3

    P1DIR |= 0xff; //端口1为输出
    BEEP = 0;
    P1SEL |= 0x01; //timer1 通道2映射口P1_0

    T1CC2H = 0x01; //20%占空比为200us
    T1CC2L = 0x77; //修改T1CC2L可调整led的亮度

    T1CC0H = 0x13; //20ms的周期时钟,频率为50ms定时器计数周期为0.000004
    T1CC0L = 0x88; //定时时长=寄存器值*定时器计数周期

    T1CCTL2 = 0x24; // 模式选择 通道2比较模式

    T1CTL = 0x02;  //模模式250KHz 1分频
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
    if (STEER_STATE == 1)
    {
        close();
    }
    else
    {
        open();
    }
    P0IFG = 0;
    P0IF = 0;
}

void servo(int angle)
{
    double time;                               //脉冲时间
    int cont;                                  //寄存器值
    time = angle * 11.1 + 510.0;               //脉冲时间与角度计算公式
    cont = (int)((time / 1000000) / 0.000004); //脉冲时间与寄存器值的计算公式

    T1CC2H = (unsigned char)(cont >> 8); //寄存器赋值
    T1CC2L = (unsigned char)cont;
}

void open(void)
{
    if (STEER_STATE == 0)
    {
        servo(1);
        Delay_ms(600);
        servo(90);
        STEER_STATE = 1;
    }
}

void close(void)
{
    if (STEER_STATE == 1)
    {
        servo(180);
        Delay_ms(600);
        servo(90);
        STEER_STATE = 0;
    }
}

void warming(void)
{
    BEEP = 1;
}
void no_warm(void)
{
    BEEP = 0;
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

unsigned int STATE(void)
{
    return STEER_STATE;
}