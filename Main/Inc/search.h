/* 
##################
    Search Run
##################
*/
#ifndef SEARCH_H
#define SEARCH_H

#include"main.h"
#include"param.h"

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

extern wall_azim wall[MAZESIZE_X][MAZESIZE_Y];
extern uint8_t step[MAZESIZE_X][MAZESIZE_Y];

void InitMaze();
void InitStep(uint8_t gx,uint8_t gy);
void CreateMap();
void SetWall();
void SearchAdachi(uint8_t gx,uint8_t gy);
dire_local GetNextDire();


#endif