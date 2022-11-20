/* 
###############################
    System Interrupt & Init
###############################
*/
#ifndef SYSTEM_H
#define SYSTEM_H

#include"main.h"
#include"analog.h"
#include"motor.h"
#include"tim.h"
#include"show.h"
#include"run.h"

void DoPanic();
void init();
void mainmenu();

extern unsigned int timer;

#endif