#include"system.h"

void system_init(){
    if(icm20648_init(&hspi2,GPIOB,GPIO_PIN_0) != 0){
        printf("ICM-20648 init failed\r\n");
        while(1);
    }
    if(as5047p_init(&hspi1,GPIOB,GPIO_PIN_2) == -1){
        printf("AS5047P init failed\r\n");
        while(1);
    }
    printf("Hello World!\r\n");
    while(1){
        printf("%d\r\n",readAngle());
        //printf("%d\r\n",accelX_raw());
        HAL_Delay(500);

        uint16_t send=0x3FFF,recv = 0;
        send |= 1 << 14;
        send |= 1 << 15;
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_RESET);
        HAL_SPI_Transmit(&hspi1,(uint8_t*)&send,1,HAL_MAX_DELAY);
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);

        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_RESET);
        HAL_SPI_Receive(&hspi1,(uint8_t*)&recv,1,HAL_MAX_DELAY);
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);
        printf("%d\r\n",recv&0x3FFF);

        HAL_Delay(500);
    }
}