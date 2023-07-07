#include"interrupt.h"

static float CalcVelocity(){
    static uint16_t rotateR = 0,rotateL = 0;

    uint16_t past_rotateR = rotateR;
    uint16_t past_rotateL = rotateL;
    rotateR = readAngle(&encR);
    rotateL = readAngle(&encL);

    int diff_rotateR = rotateR - past_rotateR;
    int diff_rotateL = rotateL - past_rotateL;

    


}


void interrupt_init(){
    HAL_TIM_Base_Start_IT(&htim6);
}

void interrupt_1ms(){
    static uint8_t output = 0;
    static uint16_t cnt = 0;

    cnt++;
    if(cnt >= 1000){
        cnt = 0;
        output++;
        if(output == 8) output = 0;
        SetLED(output);
    }
}