#ifndef DRV8212_H
#define DRV8212_H

#include "tim.h"

struct drv8212{
    TIM_HandleTypeDef* hin1;
    uint32_t in1_ch;
    TIM_HandleTypeDef* hin2;
    uint32_t in2_ch;
    uint8_t is_reverse;
};

struct opposedMotors{
    struct drv8212* hdrvR;
    struct drv8212* hdrvL;
    double pwmR;
    double pwmL;
};

void Motors_init(struct opposedMotors*);
void Motors_set(struct opposedMotors*,double,double);
void Motors_stop(struct opposedMotors*);

#endif