/* 
#############################
    Wall Sensor & voltage
#############################
*/
#ifndef ANALOG_H
#define ANALOG_H

#include"main.h"

extern uint16_t adcval[5];
extern uint16_t sensval[4];
extern float vbat;

void GetWallSens();
void GetBattVoltage();
#endif