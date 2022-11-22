/* 
##################
    Search Run
##################
*/
#ifndef SEARCH_H
#define SEARCH_H

#include"main.h"
#include"param.h"

#define _WALL_OR_NOWALL(f) ((f) ? ISWALL : NOWALL)

typedef struct{
    uint8_t north;
    uint8_t south;
    uint8_t east;
    uint8_t west;
}wall_azim;

typedef enum{
    front   = 0,
    right   = 1,
    left    = 2,
    back    = 3,
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