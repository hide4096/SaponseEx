/* 
#####################################
    Motor Control & Encoder & IMU
#####################################
*/
#ifndef MOTOR_H
#define MOTOR_H

#include"main.h"
#include"param.h"
#include<math.h>
#include"as5047p.h"
#include"icm20648.h"

extern float r_yaw_ref,angvel;
extern uint8_t motpower;
extern float spd,deg;
extern float tgt_spd,tgt_deg;
extern float angvel,r_yaw_ref;

void SetDutyRatio(int16_t,int16_t,int16_t);
void GetSpeed();
void ControlDuty();
void GetYawDeg();

#endif