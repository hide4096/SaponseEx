#include"system.h"

void system_init(){
    if(icm20648_init(&hspi2,GPIOB,GPIO_PIN_0) != 0){
        printf("ICM-20648 init failed\r\n");
        while(1);
    }

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
    if(as5047p_init(&encR) == -1 || as5047p_init(&encL) == -1){
        printf("AS5047P init failed\r\n");
        while(1);
    }

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

    Motors_init(&motors);
    Motors_set(&motors,0.0,0.0);

    printf("Hello World!\r\n\n");
    while(1){
        printf("%d\t%d\r\n",readAngle(&encR),readAngle(&encL));
        HAL_Delay(100);
    }
}