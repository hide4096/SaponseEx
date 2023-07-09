#include"peripheral.h"

struct drv8212 drvR = {
    .hin1 = &htim1,
    .hin2 = &htim2,
    .in1_ch = TIM_CHANNEL_1,
    .in2_ch = TIM_CHANNEL_3,
    .is_reverse = TRUE,
};

struct drv8212 drvL = {
    .hin1 = &htim1,
    .hin2 = &htim1,
    .in1_ch = TIM_CHANNEL_3,
    .in2_ch = TIM_CHANNEL_2,
    .is_reverse = FALSE,
};

struct opposedMotors motors = {
    .hdrvR = &drvR,
    .hdrvL = &drvL,
};

struct as5047p encR = {
    .henc = &hspi1,
    .cs_port = GPIOB,
    .cs_pin = GPIO_PIN_2,
    .is_reverse = FALSE
};

struct as5047p encL = {
    .henc = &hspi1,
    .cs_port = GPIOA,
    .cs_pin = GPIO_PIN_0,
    .is_reverse = TRUE
};