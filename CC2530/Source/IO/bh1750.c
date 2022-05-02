#include "bh1750.h"

static void delay_nus(void)
{
        int i;
        int n = 100;
        for (i = 0; i < n; i++)
        {
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
        }
}

static void delay_nms(int n)
{
        while (n--)
        {
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
        }
}

static void start_i2c(void)
{
        SDA_W();
        //LIGHT_SCK_0() ;
        //delay_nus(20);
        LIGHT_DTA_1(); //
        LIGHT_SCK_1(); //
        delay_nus();
        LIGHT_DTA_0();
        delay_nus();
        LIGHT_SCK_0();
        delay_nus();
        //delay()  ;
}

static void stop_i2c(void)
{
        SDA_W();
        LIGHT_DTA_0();
        delay_nus();
        LIGHT_SCK_1();
        delay_nus();
        LIGHT_DTA_1();
        delay_nus();
        LIGHT_SCK_0();
        delay_nus();
}

static char i2c_send(unsigned char val)
{
        int i;
        char error = 0;
        SDA_W();
        for (i = 0x80; i > 0; i /= 2)
        {
                if (val & i)
                        LIGHT_DTA_1();
                else
                        LIGHT_DTA_0();
                delay_nus();
                LIGHT_SCK_1();
                delay_nus();
                LIGHT_SCK_0();
                delay_nus();
        }
        LIGHT_DTA_1();
        SDA_R();
        //delay_nus();
        LIGHT_SCK_1();
        delay_nus();
        if (LIGHT_DTA())
                error = 1;
        delay_nus();
        LIGHT_SCK_0();
        return error;
}

static char i2c_read(char ack)
{
        int i;
        char val = 0;
        LIGHT_DTA_1();
        //SDA_R();
        for (i = 0x80; i > 0; i /= 2)
        {

                LIGHT_SCK_1();
                delay_nus();
                SDA_R();
                if (LIGHT_DTA())
                        val = (val | i);
                delay_nus();
                //SDA_R();
                LIGHT_SCK_0();
                delay_nus();
        }
        SDA_W();
        if (ack)
                LIGHT_DTA_0();
        else
                LIGHT_DTA_1();
        delay_nus();
        LIGHT_SCK_1();
        delay_nus();
        LIGHT_SCK_0();
        LIGHT_DTA_1();
        return val;
}

unsigned short get_light(void)
{
        unsigned char ack1 = 1;
        unsigned char ack2 = 1;
        unsigned char ack3 = 1;
        unsigned char ack4 = 1;
        unsigned char ack5 = 1;
        unsigned char ack6 = 1;
        unsigned char ack7 = 1;

        unsigned char t0;
        unsigned char t1;
        unsigned short t;

        P0DIR |= (1 << 1);
        delay_nms(200);

        start_i2c();
        ack1 = i2c_send(0x46);
        if (ack1)
                return 255;
        ack2 = i2c_send(0x01);
        if (ack2)
                return 254;
        stop_i2c(); //init
        start_i2c();
        ack3 = i2c_send(0x46);
        if (ack3)
                return 253;
        ack4 = i2c_send(0x01);
        if (ack4)
                return 252;
        stop_i2c(); //power
        start_i2c();
        ack5 = i2c_send(0x46);
        if (ack5)
                return 251;
        ack6 = i2c_send(0x10);
        if (ack6)
                return 250;
        stop_i2c();
        delay_nms(1500);
        start_i2c();

        ack7 = i2c_send(0x47);
        if (ack7)
                return 249;

        t0 = i2c_read(1);
        t1 = i2c_read(0);
        stop_i2c();
        t = ((short)t0) << 8;
        t |= t1;
        return t;
}
