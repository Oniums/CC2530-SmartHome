#ifndef __BH1750_H
#define __BH1750_H

#include <ioCC2530.h>

#define st(x)      do { x } while (__LINE__ == -1)
#define HAL_IO_SET(port, pin, val)        HAL_IO_SET_PREP(port, pin, val)
#define HAL_IO_SET_PREP(port, pin, val)   st( P##port##_##pin## = val; )
#define HAL_IO_GET(port, pin)   HAL_IO_GET_PREP( port,pin)
#define HAL_IO_GET_PREP(port, pin)   ( P##port##_##pin)

#define LIGHT_SCK_0()         HAL_IO_SET(0,5,0)
#define LIGHT_SCK_1()         HAL_IO_SET(0,5,1)
#define LIGHT_DTA_0()         HAL_IO_SET(0,4,0)
#define LIGHT_DTA_1()         HAL_IO_SET(0,4,1)

#define LIGHT_DTA()          HAL_IO_GET(0,4)
#define LIGHT_SCK()          HAL_IO_GET(0,5)

#define SDA_W() (P0DIR |=(1 << 4)  )
#define SDA_R() (P0DIR &=~(1 << 4) )

#define LIGHT_INIT()                        \
do{                                         \
	P0SEL &= ~0x20;                        	\
	P0DIR |=0x20;                           \
	P0_5 = 1;                               \
	                                 		\
	P0SEL &= ~0x10;                        	\
	P0DIR |= 0x10;                         	\
	P0_4 = 1; 	                            \
}while(0)
	
extern unsigned short get_light(void);

#endif

