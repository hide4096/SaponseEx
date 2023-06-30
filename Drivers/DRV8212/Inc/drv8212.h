#ifndef DRV8212_H
#define DRV8212_H

#include "tim.h"

struct drv8212{
    TIM_HandleTypeDef* htim1;
    uint32_t ch1;
    TIM_HandleTypeDef* htim2;
    uint32_t ch2;
};

struct opposedMotors{
    struct drv8212* hdrv_r;
    struct drv8212* hdrv_l;
    int16_t pwm_r;
    int16_t pwm_l;
};

void Motors_init(struct opposedMotors*);
void Motors_set(struct opposedMotors*);
void Motors_stop(struct opposedMotors*);

#endif