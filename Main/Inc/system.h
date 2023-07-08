#ifndef SYSTEM_H
#define SYSTEM_H

#include<stdio.h>
#include"peripheral.h"
#include"analog.h"

#define TRUE 1
#define FALSE 0
#define TIRE_DIAMETER 12.5

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
extern struct mouse_physical mouse;
extern struct mouse_physical target;
void system_init();
void SetLED(uint8_t);

#endif