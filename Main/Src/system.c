#include"system.h"


uint8_t use_logging = FALSE;

void system_init(){

    SetLED(0b000);

    if(icm20648_init(&hspi2,GPIOB,GPIO_PIN_0) != 0){
        printf("ICM-20648 init failed\r\n");
        while(1);
    }

    
    if(as5047p_init(&encL) == -1 || as5047p_init(&encR) == -1){
        printf("AS5047P init failed\r\n");
        while(1);
    }

    while(TRUE){
        TrigWallSensor();
        HAL_Delay(1);
        if(sensor.ll + sensor.rr >= 4000){
            break;
        }
    }
    SetLED(0b001);

    uint8_t _head_flash = &(SAVE_SECTOR);
    if(_head_flash == 0xFF){
        printf("Flash memory is empty\r\n");
        Motors_init(&motors);

        uint16_t past_rotateR = 0;
        uint16_t past_rotateL = 0;
        float spd = 0.0f,dv = 0.001f;

        interrupt_init();
        
        while(is_inloop);

        WriteMemory(&save,sizeof(save));
    }else{
        uint32_t adrs = SAVE_SECTOR;
        while(adrs < SAVE_SECTOR + SAVE_SIZE){
            struct save_data *p = (struct save_data *)adrs;
            printf("%.4f,%.4f,%.4f,%.4f\r\n",p->mouse.v,p->mouse.w,p->target.v,p->target.w);
            adrs += sizeof(struct save_data);
        }
        FlashMemory();
    }

    while(1){
        SetLED(0b111);
        HAL_Delay(500);
        SetLED(0b000);
        HAL_Delay(500);
    }
}