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
    .Kp = 7.0f,
    .Ki = 0.1f,
    .Kd = 0.0f
};

const struct gain turn = {
    .Kp = 7.0f,
    .Ki = 0.1f,
    .Kd = 0.0f
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
        //printf("v:%f w:%f\r\n",mouse.v,mouse.w);
        target.w = ((sensor.rr - sensor.ll) / (float)(sensor.rr+sensor.ll)) * 0.5f;
        target.v = (4000.-(sensor.rr+sensor.ll))*0.00005;
        printf("v:%f w:%f\r\n",target.v,target.w);
        HAL_Delay(10);
    }
}