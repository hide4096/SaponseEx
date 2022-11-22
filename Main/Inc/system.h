/* 
###############################
    System Interrupt & Init
###############################
*/
#ifndef SYSTEM_H
#define SYSTEM_H

#include"main.h"
#include"tim.h"
#include"analog.h"
#include"motor.h"
#include"show.h"
#include"run.h"
#include"search.h"

extern unsigned int timer;

void DoPanic();
void init();
void mainmenu();

#endif