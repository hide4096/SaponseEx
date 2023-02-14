/* 
################
    Odometry
################
*/

#include"odom.h"

float x=0,y=0;

void CalcPosition(){
    float rad = deg *(M_PI / 180.);
    float spd_ms = spd * 1000.;
    x+=spd_ms*DELTA_T*-sin(rad);
    y+=spd_ms*DELTA_T*cos(rad);
}