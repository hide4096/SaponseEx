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

    analog_sensing_start();

    while(sensor.ll + sensor.rr < 4000);
    SetLED(0b001);

    uint8_t _head_flash = *(uint8_t*)(SAVE_SECTOR);
    if(_head_flash == 0xFF){
        printf("Flash memory is empty\r\n");
        Motors_init(&motors);

        target.v = 0.0f;
        target.w = 0.0f;

        control_loop_start();

        uint64_t _sync;
        
        while(is_inloop){
            _sync = count;
            if(count < 500){
                target.v = 0.0f;
                target.w = 0.0f;
            }else if(count < 1000){
                target.v +=0.0006f;
                target.w = 0.0f;
            }else if(count < 1500){
                target.v = 0.3f;
                target.w = 0.0f;
            }else if(count < 2000){
                target.v -= 0.0006f;
                target.w = 0.0f;
            }else if(count < 2500){
                target.v = 0.0f;
                target.w = 0.0f;
            }
            while(_sync == count);
        }

        control_loop_stop();
        Motors_halt(&motors);

        WriteMemory(savedataHandle(),sizeof(struct save_data)*LOGGING_SIZE);
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