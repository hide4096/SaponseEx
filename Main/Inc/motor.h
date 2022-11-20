/* 
#####################################
    Motor Control & Encoder & IMU
#####################################
*/
#ifndef MOTOR_H
#define MOTOR_H

#include"main.h"
#include"tim.h"
#include"as5047p.h"
#include"icm20648.h"
#include"analog.h"
#include"show.h"

//Enc
extern AS5047P_Instance encR;
extern AS5047P_Instance encL;

//run
extern float r_yaw_ref,angvel;
extern uint8_t runmode;
extern float spd,deg,len;
extern float tgt_spd,tgt_angvel;
extern float angvel,r_yaw_ref;
extern float I_spd,I_angvel;

void SetDutyRatio(uint16_t,uint16_t,uint8_t,uint8_t);
void GetSpeed();
void ControlDuty();
void GetYawDeg();
void FailSafe();

#endif