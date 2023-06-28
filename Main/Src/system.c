#include"system.h"

void system_init(){
    /*
    if(icm20648_init(&hspi2,GPIOB,GPIO_PIN_0) != 0){
        printf("ICM-20648 init failed\r\n");
        while(1);
    }
    printf("%d\r\n",accelX_raw());
    */
    AS5047P_Instance encL, encR;
    AS5047P_Init(&encL, 0);
    AS5047P_Init(&encR, 1);
    uint8_t errcnt = 0;
    while(AS5047P_ErrorPending(&encR) || AS5047P_ErrorPending(&encL)){
        errcnt++;
        if(errcnt >= 10) while(1);
        AS5047P_Init(&encL, 0);
        AS5047P_Init(&encR, 1);
        HAL_Delay(100);
    }
    AS5047P_SetZeroPosition(&encL);
    AS5047P_SetZeroPosition(&encR);

    printf("%d\r\n",AS5047P_ReadPosition(&encL,AS5047P_OPT_ENABLED));

    printf("Hello World!\r\n");
}