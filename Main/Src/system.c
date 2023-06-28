#include"system.h"

void system_init(){
    /*
    if(icm20648_init(&hspi2,GPIOB,GPIO_PIN_0) != 0){
        printf("ICM-20648 init failed\r\n");
        while(1);
    }
    printf("%d\r\n",accelX_raw());
    */
    //AS5047P_Instance encL, encR;
    //AS5047P_Init(&encL, 0);
    //AS5047P_Init(&encR, 1);
    /*
    while(AS5047P_ErrorPending(&encR) || AS5047P_ErrorPending(&encL)){
        errcnt++;
        if(errcnt >= 10) while(1);
        AS5047P_Init(&encL, 0);
        AS5047P_Init(&encR, 1);
        HAL_Delay(100);
    }
    */
    //AS5047P_SetZeroPosition(&encL);
    //AS5047P_SetZeroPosition(&encR);

    //printf("%d\r\n",AS5047P_ReadPosition(&encL,AS5047P_OPT_ENABLED));
    //printf("%d\r\n",AS5047P_ReadPosition(&encR,AS5047P_OPT_ENABLED));

    printf("Hello World!\r\n");
    while(1){
        uint16_t send=0x3FFF,recv = 0;
        send |= 1 << 14;
        send |= 1 << 15;
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_RESET);
        //HAL_SPI_TransmitReceive(&hspi1,(uint8_t*)&send,(uint8_t*)&recv,1,HAL_MAX_DELAY);
        HAL_SPI_Transmit(&hspi1,(uint8_t*)&send,1,HAL_MAX_DELAY);
        //HAL_SPI_Receive(&hspi1,(uint8_t*)&recv,1,HAL_MAX_DELAY);
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);

        send = 0x3FFC;
        send |= 1 << 14;
        send |= 1 << 15;
        recv = 0;

        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_RESET);
        //HAL_SPI_TransmitReceive(&hspi1,(uint8_t*)&send,(uint8_t*)&recv,1,HAL_MAX_DELAY);
        //HAL_SPI_Transmit(&hspi1,(uint8_t*)&send,1,HAL_MAX_DELAY);
        HAL_SPI_Receive(&hspi1,(uint8_t*)&recv,1,HAL_MAX_DELAY);
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);
        printf("%d\r\n",recv&0x3FFF);
        HAL_Delay(500);
        //AS5047P_ReadWriteRaw(&encR, AS5047P_ANGLECOM, AS5047P_ACCESS_READ);
        //printf("%d\r\n",AS5047P_ReadWriteRaw(&encR,AS5047P_DIAAGC,AS5047P_ACCESS_READ));
    }
}