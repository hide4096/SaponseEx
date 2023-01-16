/* 
#############################
    Wall Sensor & voltage
#############################
*/
#ifndef ANALOG_H
#define ANALOG_H

#include"main.h"
#include"adc.h"

extern uint16_t adcval[5];
extern int16_t sensval[4];
extern float vbat;

void GetBattVoltage();
void FetchWallSens();

#endif