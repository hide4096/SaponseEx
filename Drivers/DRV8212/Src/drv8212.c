#include"drv8212.h"

void drv8212_init(struct drv8212* hdrv){
    HAL_TIM_PWM_Start(hdrv->htim1,hdrv->ch1);
    HAL_TIM_PWM_Start(hdrv->htim2,hdrv->ch2);
}

void drv8212_set(struct drv8212* hdrv,int16_t pwm1,int16_t pwm2){
    if(pwm1 > 0){
        __HAL_TIM_SET_COMPARE(hdrv->htim1,hdrv->ch1,pwm1);
        __HAL_TIM_SET_COMPARE(hdrv->htim1,hdrv->ch1+1,0);
    }else{
        __HAL_TIM_SET_COMPARE(hdrv->htim1,hdrv->ch1,-pwm1);
        __HAL_TIM_SET_COMPARE(hdrv->htim1,hdrv->ch1+1,1000);
    }
    if(pwm2 > 0){
        __HAL_TIM_SET_COMPARE(hdrv->htim2,hdrv->ch2,pwm2);
        __HAL_TIM_SET_COMPARE(hdrv->htim2,hdrv->ch2+1,0);
    }else{
        __HAL_TIM_SET_COMPARE(hdrv->htim2,hdrv->ch2,-pwm2);
        __HAL_TIM_SET_COMPARE(hdrv->htim2,hdrv->ch2+1,1000);
    }
}
void drv8212_stop(struct drv8212* hdrv){
    __HAL_TIM_SET_COMPARE(hdrv->htim1,hdrv->ch1,0);
    __HAL_TIM_SET_COMPARE(hdrv->htim1,hdrv->ch1,0);
    __HAL_TIM_SET_COMPARE(hdrv->htim2,hdrv->ch2,0);
    __HAL_TIM_SET_COMPARE(hdrv->htim2,hdrv->ch2,0);
}

void Motors_init(struct opposedMotors* hmtrs){
    drv8212_init(hmtrs->hdrv_r);
    drv8212_init(hmtrs->hdrv_l);
}

void Motors_set(struct opposedMotors* hmtrs){
    drv8212_set(hmtrs->hdrv_r,hmtrs->pwm_r,hmtrs->pwm_r);
    drv8212_set(hmtrs->hdrv_l,hmtrs->pwm_l,hmtrs->pwm_l);
}

void Motors_stop(struct opposedMotors* hmtrs){
    drv8212_stop(hmtrs->hdrv_r);
    drv8212_stop(hmtrs->hdrv_l);
}