#include "ioCC2530.h"
typedef unsigned short uint16;
uint16 ReadGasData(void);

uint16 ReadGasData(void)
{
    uint16 reading = 0;
    /* Enable channel */
    ADCCFG |= 0x40;
    /* writing to this register starts the extra conversion */
    ADCCON3 = 0x86; // P0_6 ADDC5： 64 抽取率(7 位ENOB)  0110： AIN6
    /* Wait for the conversion to be done */
    while (!(ADCCON1 & 0x80))
        ;
    /* Disable channel after done conversion */
    ADCCFG &= (0x40 ^ 0xFF); //按位异或。如1010^1111=0101（二进制）
    /* Read the result */
    reading = ADCL;
    reading |= (int)(ADCH << 8);
    reading >>= 6;
    return (reading);
}
