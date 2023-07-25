#include"system.h"

struct mouse_physical target = {
    .v = 0,
    .w = 0
};

struct mouse_physical mouse = {
    .v = 0,
    .w = 0
};

const struct gain straight = {
    .Kp = 5.0f,
    .Ki = 0.01f,
    .Kd = 0.0f
};

const struct gain turn = {
    .Kp = 5.0f,
    .Ki = 0.01f,
    .Kd = 0.0f
};

const struct mouse_feature sapoex = {
    .diam = 12.5f,
    .m = 27.4f,
    .gear_ratio = 4.2222,
    .ke = 0.062f,
    .r = 5.0f,
    .kT = 0.594f
};


void SetLED(uint8_t state){
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,!(state&0b001));
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_14,!(state&0b010));
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_15,!(state&0b100));
}

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

    Motors_init(&motors);

    interrupt_init();

    printf("Hello World!\r\n\n");
    uint16_t past_rotateR = 0;
    uint16_t past_rotateL = 0;
    float spd = 0.0f,dv = 0.001f;

    HAL_Delay(100);

    
    while(1){
        //target.w = ((sensor.rr - sensor.ll) / (float)(sensor.rr+sensor.ll)) * 0.8f;
        //target.v = (3000.-(sensor.rr+sensor.ll))*0.0003;
        target.v = 0.0f;
        target.w = 0.0f;
        //printf("v:%f w:%f\r\n",target.v,target.w);
        //printf("%d\t%d\r\n",readAngle(&encR),readAngle(&encL));
        HAL_Delay(100);

        /*
        uint16_t rotateR = readAngle(&encR);
        uint16_t rotateL = readAngle(&encL);

        int diff_rotateR = rotateR - past_rotateR;
        int diff_rotateL = rotateL - past_rotateL;

        if(diff_rotateR > AS5047P_MAX_ANGLE/2) diff_rotateR -= AS5047P_MAX_ANGLE;
        else if(diff_rotateR < -AS5047P_MAX_ANGLE/2) diff_rotateR += AS5047P_MAX_ANGLE;
        if(diff_rotateL > AS5047P_MAX_ANGLE/2) diff_rotateL -= AS5047P_MAX_ANGLE;
        else if(diff_rotateL < -AS5047P_MAX_ANGLE/2) diff_rotateL += AS5047P_MAX_ANGLE;
        past_rotateR = rotateR;
        past_rotateL = rotateL;


        spd+=dv;
        if(spd > 1.0f || spd < -1.0f) dv*=-1.0f;
        Motors_set(&motors,spd,spd);
        ITM_SendChar((diff_rotateR>>4)+128,1);
        ITM_SendChar((diff_rotateL>>4)+128,2);
        ITM_SendChar((spd*128)+127,3);
        printf("%d\t%d\t%f\r\n",diff_rotateR,diff_rotateL,spd);
        HAL_Delay(1);
        */
    }
}