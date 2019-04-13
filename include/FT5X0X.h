#ifndef __FT5X0X_H__
#define __FT5X0X_H__

#include <FreeRTOS.h>



extern int ft5x0x_init(void);
extern void ft5x0x_read_touch_x_y(uint16_t *x,uint16_t *y);

#endif