/* 
##################
    Search Run
##################
*/

#include"search.h"

wall_azim wall[MAZESIZE_X][MAZESIZE_Y];
uint8_t step[MAZESIZE_X][MAZESIZE_Y];

void InitMaze(){
    for(int x=0;x<MAZESIZE_X;x++){
        for(int y=0;y<MAZESIZE_Y;y++){
            wall[x][y].north = wall[x][y].north = wall[x][y].north = wall[x][y].north = UNKNOWN;
            if(x == 0)              wall[x][y].west = ISWALL;
            if(x == MAZESIZE_X-1)   wall[x][y].east = ISWALL;
            if(y == 0)              wall[x][y].south = ISWALL;
            if(y == MAZESIZE_Y-1)   wall[x][y].north = ISWALL;
        }
    }

    wall[0][0].east = wall[1][0].west = ISWALL;
}

void InitStep(uint8_t gx,uint8_t gy){
    for(int x=0;i<MAZESIZE_X:x++){
        for(int y=0;i<MAZESIZE_Y;y++){
            step[x][y] = 255;
        }
    }
    step[gx][gy] = 0;
}

void CreateMap(uint8_t gx,uint8_t gy,uint8_t mask){
    InitStep(gx,gy);

    uint8_t is_changed = 1;

    while(is_changed){
        uint8_t is_changed = 0;
        for(int x=0;x<MAZESIZE_X;x++){
            for(int y=0;y<MAZESIZE_Y;y++){
                //ステップ数計算してないマスはパス
                if(step[x][y] == 255) continue;
                
                //北
                if(y<MAZESIZE_Y-1){
                    if((wall[x][y].north & mask) == NOWALL){
                        if(step[x][y+1] == 255){
                            step[x][y+1] = step[x][y]+1;
                            is_create_now = 1;
                        }
                    }
                }
                
                //東
                if(x<MAZESIZE_X-1){
                    if((wall[x][y].east & mask) == NOWALL){
                        if(step[x+1][y] == 255){
                            step[x+1][y] = step[x][y]+1;
                            is_create_now = 1;
                        }
                    }
                }
                
                //南
                if(y>0){
                    if((wall[x][y].south & mask) == NOWALL){
                        if(step[x][y-1] == 255){
                            step[x][y-1] = step[x][y]+1;
                            is_create_now = 1;
                        }
                    }
                }
                
                //西
                if(x>0){
                    if((wall[x][y].west & mask) == NOWALL){
                        if(step[x-1][y] == 255){
                            step[x-1][y] = step[x][y]+1;
                            is_create_now = 1;
                        }
                    }
                }
            }
        }
    }
}

dire_local GetNextDire(uint8_t gx,uint8_t gy,uint8_t mask,dire_global* _dir){
    CreateMap(gx,gy,mask);
    if((wall[]))

}

void SearchAdachi(uint8_t gx,uint8_t gy){
    uint8_t nextdire;
    switch(GetNextDire()){
        case front:
            Straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);
            break;
        case right:
            SpinTurn(90,TURN_ACCEL,TURN_SPEED,RIGHT);
            Straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);
        case left:
            SpinTurn(90,TURN_ACCEL,TURN_SPEED,LEFT);
            Straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);
            break;
        case back:
            SpinTurn(180,TURN_ACCEL,TURN_SPEED,RIGHT);
            Straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);
            break;
    }


}