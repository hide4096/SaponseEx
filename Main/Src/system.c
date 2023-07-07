#include"system.h"

struct mouse_physical target = {
    .v = 0,
    .w = 0
};

struct mouse_physical mouse = {
    .v = 0,
    .w = 0
};

const struct gain straight = {
    .Kp = 2.0f,
    .Ki = 0.001f,
    .Kd = 0.0f
};

const struct gain turn = {
    .Kp = 2.0f,
    .Ki = 0.001f,
    .Kd = 0.0f
};

struct drv8212 drvR = {
    .hin1 = &htim1,
    .hin2 = &htim2,
    .in1_ch = TIM_CHANNEL_1,
    .in2_ch = TIM_CHANNEL_3,
    .is_reverse = FALSE
};

struct drv8212 drvL = {
    .hin1 = &htim1,
    .hin2 = &htim1,
    .in1_ch = TIM_CHANNEL_3,
    .in2_ch = TIM_CHANNEL_2,
    .is_reverse = TRUE
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

void SetLED(uint8_t state){
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,!(state&0b001));
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_14,!(state&0b010));
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_15,!(state&0b100));
}

void system_init(){

    SetLED(0b000);

    if(icm20648_init(&hspi2,GPIOB,GPIO_PIN_0) != 0){
        printf("ICM-20648 init failed\r\n");
        while(1);
    }

    
    if(as5047p_init(&encR) == -1 || as5047p_init(&encL) == -1){
        printf("AS5047P init failed\r\n");
        while(1);
    }

    Motors_init(&motors);

    interrupt_init();

    printf("Hello World!\r\n\n");
    while(1){
        printf("v:%f w:%f\r\n",mouse.v,mouse.w);
        HAL_Delay(100);
    }
}