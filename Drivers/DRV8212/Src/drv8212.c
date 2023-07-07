#include"drv8212.h"

static void drv8212_init(struct drv8212* hdrv){
    HAL_TIM_PWM_Start(hdrv->hin1,hdrv->in1_ch);
    HAL_TIM_PWM_Start(hdrv->hin2,hdrv->in2_ch);
}

static void drv8212_set(struct drv8212* hdrv,double _ratio){
    uint32_t max1 = hdrv->hin1->Init.Period;
    uint32_t max2 = hdrv->hin2->Init.Period;

    double dutyratio = fabs(_ratio);
    if(dutyratio > 1.) dutyratio = 1.;

    if((_ratio > 0.) ^ (hdrv->is_reverse)){
        __HAL_TIM_SET_COMPARE(hdrv->hin1,hdrv->in1_ch,max1);
        __HAL_TIM_SET_COMPARE(hdrv->hin2,hdrv->in2_ch,dutyratio*max2);
    }else{
        __HAL_TIM_SET_COMPARE(hdrv->hin1,hdrv->in1_ch,dutyratio*max1);
        __HAL_TIM_SET_COMPARE(hdrv->hin1,hdrv->in2_ch,max2);
    }
}
static void drv8212_stop(struct drv8212* hdrv){
    HAL_TIM_PWM_Stop(hdrv->hin1,hdrv->in1_ch);
    HAL_TIM_PWM_Stop(hdrv->hin2,hdrv->in2_ch);
}

void Motors_init(struct opposedMotors* hmtrs){
    drv8212_init(hmtrs->hdrvR);
    drv8212_init(hmtrs->hdrvL);
    drv8212_set(hmtrs->hdrvR,0.);
    drv8212_set(hmtrs->hdrvL,0.);
}

void Motors_set(struct opposedMotors* hmtrs,double _dutyratioR,double _dutyratioL){
    drv8212_set(hmtrs->hdrvR,_dutyratioR);
    drv8212_set(hmtrs->hdrvL,_dutyratioL);
}

void Motors_stop(struct opposedMotors* hmtrs){
    drv8212_set(hmtrs->hdrvR,0.);
    drv8212_set(hmtrs->hdrvL,0.);
    drv8212_stop(hmtrs->hdrvR);
    drv8212_stop(hmtrs->hdrvL);
}