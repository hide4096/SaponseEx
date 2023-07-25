#ifndef INTERRUPT_H
#define INTERRUPT_H

#include"system.h"
#include"tim.h"
#include"analog.h"

extern uint8_t is_inloop;
extern struct save_data save[16384];

void interrupt_init();
void interrupt_1ms();

#endif