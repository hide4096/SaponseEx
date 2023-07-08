#ifndef PERIPHERAL_H
#define PERIPHERAL_H

#include"icm20648.h"
#include"as5047p.h"
#include"drv8212.h"


extern struct opposedMotors motors;
extern struct as5047p encR;
extern struct as5047p encL;
extern struct drv8212 drvR;
extern struct drv8212 drvL;

#endif