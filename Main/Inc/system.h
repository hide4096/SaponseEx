#ifndef SYSTEM_H
#define SYSTEM_H

#include<stdio.h>
#include"icm20648.h"
#include"as5047p.h"
#include"drv8212.h"

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

extern struct mouse_physical mouse;
extern struct mouse_physical target;
extern struct opposedMotors motors;
extern struct as5047p encR;
extern struct as5047p encL;
extern struct drv8212 drvR;
extern struct drv8212 drvL;
extern const struct gain straight;
extern const struct gain turn;

void system_init();
void SetLED(uint8_t);

#endif