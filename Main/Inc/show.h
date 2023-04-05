/* 
########################
    Show Information
########################
*/
#ifndef SHOW_H
#define SHOW_H

#include"main.h"
#include"motor.h"

extern uint8_t led;

void Blink(uint16_t val);
void SetLED();
void DoPanic();

#endif