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

struct gain{
    float Kp;
    float Ki;
    float Kd;
};


extern const struct gain straight;
extern const struct gain turn;
extern const struct mouse_feature sapoex;
extern struct mouse_physical mouse;
extern struct mouse_physical target;

void system_init();
void SetLED(uint8_t);

#endif