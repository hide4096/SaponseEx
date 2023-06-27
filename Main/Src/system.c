#include"system.h"

void system_init(){
    if(icm20648_init(&hspi2,GPIOB,GPIO_PIN_0) != 0){
        printf("ICM-20648 init failed\r\n");
        while(1);
    }
    printf("0x%x\r\n",WHOAMI());
    printf("Hello World!\r\n");
}