#ifndef __DHT11_H__
#define __DHT11_H__

#define uchar unsigned char
extern void Delay_ms(unsigned int xms);	//延时函数
extern void COM(void);                  // 温湿写入
extern void DHT11(uchar *temper,uchar *humid);                //温湿传感启动

#endif