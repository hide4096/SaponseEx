#ifndef AS5047P_H
#define AS5047P_H

#include"spi.h"

int as5047p_init(SPI_HandleTypeDef*,GPIO_TypeDef*,uint16_t);
float as5047p_getAngle(void);

#endif