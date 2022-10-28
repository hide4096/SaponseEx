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

//Enc
extern AS5047P_Instance encR;
extern AS5047P_Instance encL;

//run
extern float r_yaw_ref,angvel;
extern uint8_t motpower;
extern float spd,deg;
extern float tgt_spd,tgt_angvel;
extern float angvel,r_yaw_ref;
extern float I_spd,I_angvel;

void SetDutyRatio(int16_t,int16_t);
void GetSpeed();
void ControlDuty();
void GetYawDeg();
void FailSafe();

#endif