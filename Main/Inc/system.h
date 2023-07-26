#ifndef SYSTEM_H
#define SYSTEM_H

#include<stdio.h>
#include"peripheral.h"
#include"analog.h"

#define TRUE 1
#define FALSE 0

struct mouse_feature{
    float diam;
    float m;
    float gear_ratio;
    float ke;
    float r;
    float kT;
};

struct mouse_physical{
    float v;
    float w;
};

struct save_data{
    struct mouse_physical mouse;
    struct mouse_physical target;
    uint64_t count;
    float fbR;
    float fbL;
    float ffR;
    float ffL;
};

struct gain{
    float Kp;
    float Ki;
    float Kd;
};

struct ffgain{
    float ka;
    float ka2;
    float kv;
};

extern uint8_t use_logging;

void system_init();

#endif