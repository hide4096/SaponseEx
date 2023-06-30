#ifndef AS5047P_H
#define AS5047P_H

#include"spi.h"

enum direction{
    CW,
    CCW
};
struct as5047p{
    SPI_HandleTypeDef* henc;
    GPIO_TypeDef* cs_port;
    uint8_t direction;
    uint16_t cs_pin;
};

int as5047p_init(struct as5047p*);
int16_t readAngle(struct as5047p*);

#endif