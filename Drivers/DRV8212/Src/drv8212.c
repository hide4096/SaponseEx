#include"drv8212.h"

static void drv8212_init(struct drv8212* hdrv){
    HAL_TIM_PWM_Start(hdrv->hin1,hdrv->in1_ch);
    HAL_TIM_PWM_Start(hdrv->hin2,hdrv->in2_ch);
}

static void drv8212_set(struct drv8212* hdrv,float _dutyratio){
    double ratio = _dutyratio;
    if(ratio > 1.) ratio = 1.;
    else if(ratio < -1.) ratio = -1.;

    uint32_t max1 = hdrv->hin1->Init.Period;
    uint32_t max2 = hdrv->hin2->Init.Period;

    if((ratio > 0.) ^ (hdrv->is_reverse)){
        __HAL_TIM_SET_COMPARE(hdrv->hin1,hdrv->in1_ch,max1);
        __HAL_TIM_SET_COMPARE(hdrv->hin2,hdrv->in2_ch,ratio*max2);
    }else{
        __HAL_TIM_SET_COMPARE(hdrv->hin1,hdrv->in1_ch,-ratio*max1);
        __HAL_TIM_SET_COMPARE(hdrv->hin1,hdrv->in2_ch,max2);
    }
}
static void drv8212_stop(struct drv8212* hdrv){
    HAL_TIM_PWM_Stop(hdrv->hin1,hdrv->in1_ch);
    HAL_TIM_PWM_Stop(hdrv->hin2,hdrv->in2_ch);
}

void Motors_init(struct opposedMotors* hmtrs){
    drv8212_init(hmtrs->hdrv_r);
    drv8212_init(hmtrs->hdrv_l);
    drv8212_set(hmtrs->hdrv_r,0.);
    drv8212_set(hmtrs->hdrv_l,0.);
}

void Motors_set(struct opposedMotors* hmtrs){
    drv8212_set(hmtrs->hdrv_r,hmtrs->pwm_r,hmtrs->pwm_r);
    drv8212_set(hmtrs->hdrv_l,hmtrs->pwm_l,hmtrs->pwm_l);
}

void Motors_stop(struct opposedMotors* hmtrs){
    drv8212_set(hmtrs->hdrv_r,0.);
    drv8212_set(hmtrs->hdrv_l,0.);
    drv8212_stop(hmtrs->hdrv_r);
    drv8212_stop(hmtrs->hdrv_l);
}