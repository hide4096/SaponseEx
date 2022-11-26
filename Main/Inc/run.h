/* 
####################
    Run and Turn
####################
*/
#ifndef RUN_H
#define RUN_H

#include"main.h"
#include"motor.h"

void Straight(float tgt_len,float _accel,float _max_spd,float _end_spd);
void SpinTurn(float _deg,float _ang_accel,float _max_angvel,int8_t _dir);

#endif