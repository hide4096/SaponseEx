#ifndef SYSTEM_H
#define SYSTEM_H

#include<stdio.h>
#include"icm20648.h"
#include"as5047p.h"
#include"drv8212.h"

#define TRUE 1
#define FALSE 0

struct mouse_physical{
    float v;
    float w;

};

extern mouse_physical mouse;
extern opposedMotors motors;
extern as5047p encR;
extern as5047p encL;
extern drv8212 drvR;
extern drv8212 drvL;

void system_init();
void SetLED(uint8_t);

#endif