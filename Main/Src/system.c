#include"system.h"

void system_init(){
    if(icm20648_init(&hspi2,GPIOB,GPIO_PIN_0) != 0){
        printf("ICM-20648 init failed\r\n");
        while(1);
    }
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);
    if(as5047p_init(&hspi1,GPIOB,GPIO_PIN_2) == -1){
        printf("AS5047P init failed\r\n");
        while(1);
    }
    printf("Hello World!\r\n");
}