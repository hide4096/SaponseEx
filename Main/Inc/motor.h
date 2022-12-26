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
extern int8_t turndir;
extern float spd,deg,len,accel,max_spd,ang_accel;
extern float max_angvel;
extern float tvR,tvL;
extern float angvel,r_yaw_ref;
extern float I_spd,I_angvel;
extern float tgt_spd,tgt_angvel;
extern int I_error;
extern uint8_t wallfix_is;
extern int16_t encR_val,encL_val;
extern int16_t d_encR_val,d_encL_val;

void SetDutyRatio(float motL,float motR,uint8_t motR_isCW,uint8_t motL_isCW);
void GetSpeed();
void ControlDuty();
void GetYawDeg();
void FailSafe();

#endif