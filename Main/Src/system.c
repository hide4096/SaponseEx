#include"system.h"

void system_init(){
    if(icm20648_init(&hspi2,GPIOB,GPIO_PIN_0) != 0){
        printf("ICM-20648 init failed\r\n");
        while(1);
    }
    struct as5047p encR = {
        .henc = &hspi1,
        .cs_port = GPIOB,
        .cs_pin = GPIO_PIN_2
    };
    struct as5047p encL = {
        .henc = &hspi1,
        .cs_port = GPIOA,
        .cs_pin = GPIO_PIN_0
    };
    if(as5047p_init(&encR) == -1 || as5047p_init(&encL) == -1){
        printf("AS5047P init failed\r\n");
        while(1);
    }
    printf("Hello World!\r\n");
    while(1){
        printf("%d\r\n",readAngle(&encR));
        printf("%d\r\n",readAngle(&encL));
        HAL_Delay(500);
    }
}