#ifndef __ABTXINIT_H__
#define	__ABTXINIT_H__  

#include "lib_gpio.h"



#define IO_S1     26    //GPIOD0  接听 GPIO0[26]
#define IO_S2     27    //GPIOD1  开锁 GPIO0[27]
#define IO_S3     24    //GPIOD2  监视 GPIO0[24]
#define IO_S4     25    //GPIOD3  中心 GPIO0[25]
#define IO_S4     25    //GPIOD3  中心 GPIO0[25]
#define PWM0IO    60    //GPIO_1[28]


#define ACE_RST   0x0A    //GPIO_0[10]=0*32+10=A
#define ACR_POWER 0x0B    //GPIO_0[11]=0*32+11=B
#define PD_SPK  0x0d //GPIO_0[13]
#define opEN() gm_gpio_set_value(PD_SPK,SET_HIGH)   //开启功放
#define opDS() gm_gpio_set_value(PD_SPK,SET_LOW)  //关闭功放


extern void ace_init(void);
extern void clean_watchdog(void);
extern uint8_t keyboard_scan(void);


#endif



