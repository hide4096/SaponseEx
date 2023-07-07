#ifndef AS5047P_H
#define AS5047P_H

#include"spi.h"

#define AS5047P_MAX_ANGLE 0x3FFF

struct as5047p{
    SPI_HandleTypeDef* henc;
    GPIO_TypeDef* cs_port;
    uint16_t cs_pin;
    uint8_t is_reverse;
};

int as5047p_init(struct as5047p*);
int16_t readAngle(struct as5047p*);

#endif