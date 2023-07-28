#ifndef RUN_H
#define RUN_H

#include"main.h"

#define SEARCH_SPEED 0.4
#define SEARCH_ACCEL 1.0
#define TURN_SPEED  600.
#define TURN_ACCEL  2000.

#define HALF_SECTION 45
#define FULL_SECTION 90

#define RIGHT 1
#define LEFT -1

#define MIN_ANGVEL 150.0f
#define MIN_SPEED 0.1f

#define STRAIGHT_MODE 1
#define TURN_MODE 2

#define DELAY 100

void Straight(float tgt_len,float _accel,float _max_spd,float _end_spd);
void SpinTurn(float _deg,float _ang_accel,float _max_angvel,int8_t _dir);

#endif