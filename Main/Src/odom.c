/* 
################
    Odometry
################
*/

#include"odom.h"

float pos_x=0,pos_y=0;

void CalcPosition(){
    float rad = deg *(M_PI / 180.);
    float spd_ms = spd * 1000.;
    pos_x+=spd_ms*DELTA_T*-sin(rad);
    pos_y+=spd_ms*DELTA_T*cos(rad);
}