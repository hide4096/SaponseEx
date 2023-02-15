/* 
################
    Odometry
################
*/
#ifndef ODOM_H
#define ODOM_H

#include"main.h"

extern float pos_x,pos_y;
extern uint32_t chase_phase;

void CalcPosition();
void GenerateChase();

#endif