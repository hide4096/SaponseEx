#ifndef SEARCH_H
#define SEARCH_H

#include"main.h"
#include"run.h"

#define _WALL_OR_NOWALL(f) ((f) ? ISWALL : NOWALL)
#define MAZESIZE_X 32
#define MAZESIZE_Y 32

#define NOWALL 0b00
#define ISWALL 0b01
#define UNKNOWN 0b11

#define SEARCH_MASK 0b01

typedef struct{
    uint8_t north;
    uint8_t south;
    uint8_t east;
    uint8_t west;
}wall_azim;

typedef enum{
    front   = 0,
    right   = 1,
    back    = 2,
    left    = 3,
}dire_local;

typedef enum{
    north   = 0,
    east    = 1,
    south   = 2,
    west    = 3,
}dire_global;

extern uint8_t x_mypos;
extern uint8_t y_mypos;
extern dire_global dire_mypos;

extern wall_azim wall[MAZESIZE_X][MAZESIZE_Y];
extern uint8_t step[MAZESIZE_X][MAZESIZE_Y];

void InitMaze();
void InitStep(uint8_t gx,uint8_t gy);
void CreateMap(uint8_t gx,uint8_t gy,uint8_t mask);
void SetWall(uint8_t x,uint8_t y);
void SearchAdachi(uint8_t gx,uint8_t gy);
dire_local GetNextDire(uint8_t gx,uint8_t gy,uint8_t mask,dire_global* dire);
uint8_t GetPriority(uint8_t x,uint8_t y,dire_global dire);

#endif