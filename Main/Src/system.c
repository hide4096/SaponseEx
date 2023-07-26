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
    control_loop_start();
    HAL_Delay(10);
    printf("%.2fV\r\n",sensor.vbat);
    if(sensor.vbat > 4.0f){
        SetLED(0b100);
    }else if(sensor.vbat > 3.8f){
        SetLED(0b010);
    }else if(sensor.vbat > 3.6f){
        SetLED(0b001);
    }else{
        SetLED(0b000);
        while(1);
    }

    HAL_Delay(1000);
    SetLED(0b000);
    HAL_Delay(100);

    uint8_t slot = 0b001;
    while(sensor.ll + sensor.rr < 4000){
        if(mouse.v > 0.05f){
            if(slot >= 0b111) slot = 0b001;
            else slot++;
            SetLED(0b000);
            HAL_Delay(500);
        }else if(mouse.v < -0.05f){
            if(slot <= 0b001) slot = 0b111;
            else slot--;
            SetLED(0b000);
            HAL_Delay(500);
        }
        SetLED(slot);
    }
    SetLED(0b000);
    HAL_Delay(500);

    uint8_t _head_flash = *(uint8_t*)(SAVE_SECTOR);
    if(_head_flash == 0xFF && slot > 0){
        printf("Flash memory is empty\r\n");
        control_loop_stop();
        Motors_init(&motors);

        target.v = 0.0f;
        target.w = 0.0f;
        mouse.v = 0.0f;
        mouse.w = 0.0f;

        use_logging = TRUE;
        control_loop_start();

        uint64_t _sync;
        
        while(count < LOGGING_SIZE){
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
        printf("Start Write\r\n");
        WriteMemory(savedataHandle(),sizeof(struct save_data)*LOGGING_SIZE);
        printf("End Write\r\n");
    }else{
        uint32_t _read = 0;
        while(_read < SAVE_SIZE && _read < sizeof(struct save_data)*LOGGING_SIZE){
            struct save_data *p = (struct save_data *)(_read+SAVE_SECTOR);
            int seq = _read/sizeof(struct save_data);
            printf("%d,%.4f,%.4f,%.4f,%.4f",seq,p->mouse.v,p->mouse.w,p->target.v,p->target.w);
            printf(",%.4f,%.4f,%.4f,%.4f\r\n",p->fbR,p->fbL,p->ffR,p->ffL);
            _read += sizeof(struct save_data);
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