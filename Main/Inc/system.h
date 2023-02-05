/* 
###############################
    System Interrupt & Init
###############################
*/
#ifndef SYSTEM_H
#define SYSTEM_H

#include"main.h"
#include"tim.h"
#include"usart.h"
#include"analog.h"
#include"motor.h"
#include"show.h"
#include"run.h"
#include"search.h"
#include"memory.h"

extern unsigned int timer;
extern uint32_t cnt;
extern int32_t save[4][4096];

void DoPanic();
void init();
void mainmenu();

#endif