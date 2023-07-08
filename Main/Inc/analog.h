#ifndef ANALOG_H
#define ANALOG_H

#include"adc.h"
struct analog{
    uint16_t ll,l,r,rr;
    float vbat;
};


extern struct analog sensor;

float GetBattVoltage();
void TrigWallSens();

#endif