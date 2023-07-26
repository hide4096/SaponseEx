#ifndef INTERRUPT_H
#define INTERRUPT_H

#include"system.h"
#include"tim.h"
#include"analog.h"

#define LOGGING_SIZE 5000

extern uint8_t is_inloop;
extern uint64_t count;

void control_loop_start();
void control_loop_stop();
void analog_sensing_start();
void analog_sensing_stop();
void interrupt_1ms();
void interrupt_500us();
struct save_data* savedataHandle();

#endif