/* 
##################
    Search Run
##################
*/

#include"search.h"

wall_azim wall[MAZESIZE_X][MAZESIZE_Y];
uint8_t step[MAZESIZE_X][MAZESIZE_Y];
uint8_t x_mypos=0;
uint8_t y_mypos=0;
dire_global dire_mypos = north;

void InitMaze(){
    for(int x=0;x<MAZESIZE_X;x++){
        for(int y=0;y<MAZESIZE_Y;y++){
            wall[x][y].north = wall[x][y].east = wall[x][y].south = wall[x][y].west = UNKNOWN;
            if(x == 0)              wall[x][y].west = ISWALL;
            if(x == MAZESIZE_X-1)   wall[x][y].east = ISWALL;
            if(y == 0)              wall[x][y].south = ISWALL;
            if(y == MAZESIZE_Y-1)   wall[x][y].north = ISWALL;
        }
    }

    wall[0][0].east = wall[1][0].west = ISWALL;
}

void InitStep(uint8_t gx,uint8_t gy){
    for(int x=0;x<MAZESIZE_X;x++){
        for(int y=0;y<MAZESIZE_Y;y++){
            step[x][y] = 255;
        }
    }
    step[gx][gy] = 0;
}

void CreateMap(uint8_t gx,uint8_t gy,uint8_t mask){
    InitStep(gx,gy);

    uint8_t is_changed = 1;

    while(is_changed){
        is_changed = 0;
        for(int x=0;x<MAZESIZE_X;x++){
            for(int y=0;y<MAZESIZE_Y;y++){
                //ステップ数計算してないマスはパス
                if(step[x][y] == 255) continue;
                
                //北
                if(y<MAZESIZE_Y-1){
                    if((wall[x][y].north & mask) == NOWALL){
                        if(step[x][y+1] == 255){
                            step[x][y+1] = step[x][y]+1;
                            is_changed = 1;
                        }
                    }
                    
                }
                
                //東
                if(x<MAZESIZE_X-1){
                    if((wall[x][y].east & mask) == NOWALL){
                        if(step[x+1][y] == 255){
                            step[x+1][y] = step[x][y]+1;
                            is_changed = 1;
                        }
                    }
                }
                
                //南
                if(y>0){
                    if((wall[x][y].south & mask) == NOWALL){
                        if(step[x][y-1] == 255){
                            step[x][y-1] = step[x][y]+1;
                            is_changed = 1;
                        }
                    }
                }
                
                //西
                if(x>0){
                    if((wall[x][y].west & mask) == NOWALL){
                        if(step[x-1][y] == 255){
                            step[x-1][y] = step[x][y]+1;
                            is_changed = 1;
                        }
                    }
                }
            }
        }
    }
}

uint8_t GetPriority(uint8_t x,uint8_t y,dire_global dire){
    uint8_t priority = 0;

    if(dire_mypos == dire){
		priority = 2;
	}else if( ((4+dire_mypos-dire)%4) == 2){
		priority = 0;
	}else{
		priority = 1;
	}
    
    wall_azim w = wall[x][y];
    if((w.north == UNKNOWN) || (w.east == UNKNOWN) || (w.south == UNKNOWN) || (w.west == UNKNOWN)){
        priority+=4;
    }

	return priority;
}

void SetWall(uint8_t x,uint8_t y){
    wall_azim w;
    switch(dire_mypos){
        case north:
            w.north = _WALL_OR_NOWALL(sensval[SL] > WALL_TH_L || sensval[SSR] > WALL_TH_R);
            if(w.north == ISWALL) led = 0b101;
            else led = 0b000;
            w.east  = _WALL_OR_NOWALL(sensval[FR] > WALL_TH_FR);
            w.west  = _WALL_OR_NOWALL(sensval[FL] > WALL_TH_FL);
            w.south = NOWALL;
            break;
        case south:
            w.south = _WALL_OR_NOWALL(sensval[SL] > WALL_TH_L || sensval[SSR] > WALL_TH_R);
            w.west  = _WALL_OR_NOWALL(sensval[FR] > WALL_TH_FR);
            w.east  = _WALL_OR_NOWALL(sensval[FL] > WALL_TH_FL);
            w.north = NOWALL;
            break;
        case east:
            w.east  = _WALL_OR_NOWALL(sensval[SL] > WALL_TH_L || sensval[SSR] > WALL_TH_R);
            w.south = _WALL_OR_NOWALL(sensval[FR] > WALL_TH_FR);
            w.north = _WALL_OR_NOWALL(sensval[FL] > WALL_TH_FL);
            w.west  = NOWALL;
            break;
        case west:
            w.west  = _WALL_OR_NOWALL(sensval[SL] > WALL_TH_L || sensval[SSR] > WALL_TH_R);
            w.north = _WALL_OR_NOWALL(sensval[FR] > WALL_TH_FR);
            w.south = _WALL_OR_NOWALL(sensval[FL] > WALL_TH_FL);
            w.east  = NOWALL;
            break;
    }
    wall[x][y] = w;

    if(x < MAZESIZE_X-1) wall[x+1][y].west  = w.east;
    if(y < MAZESIZE_Y-1) wall[x][y+1].north = w.south;
    if(x > 0)            wall[x-1][y].east  = w.west;         
    if(y > 0)            wall[x][y-1].south  = w.north;         
}

dire_local GetNextDire(uint8_t gx,uint8_t gy,uint8_t mask,dire_global* dire){
    CreateMap(gx,gy,mask);
    uint8_t tmp_priority,priority = 0;
    uint8_t little = 255;

    if((wall[x_mypos][y_mypos].north & mask) == NOWALL){
        tmp_priority = GetPriority(x_mypos,y_mypos+1,north);
        uint8_t nextstep = step[x_mypos][y_mypos+1];
        if(nextstep < little){
            little = nextstep;
            *dire = north;
            priority = tmp_priority;
        }else if(nextstep == little){
            if(priority < tmp_priority){
                *dire = north;
                priority = tmp_priority;
            }
        }
    }

    if((wall[x_mypos][y_mypos].east & mask) == NOWALL){
        tmp_priority = GetPriority(x_mypos+1,y_mypos,east);
        uint8_t nextstep = step[x_mypos+1][y_mypos];
        if(nextstep < little){
            little = nextstep;
            *dire = east;
            priority = tmp_priority;
        }else if(nextstep == little){
            if(priority < tmp_priority){
                *dire = east;
                priority = tmp_priority;
            }
        }
    }

    if((wall[x_mypos][y_mypos].south  & mask) == NOWALL){
        tmp_priority = GetPriority(x_mypos,y_mypos-1,south);
        uint8_t nextstep = step[x_mypos][y_mypos-1];
        if(nextstep < little){
            little = nextstep;
            *dire = south;
            priority = tmp_priority;
        }else if(nextstep == little){
            if(priority < tmp_priority){
                *dire = south;
                priority = tmp_priority;
            }
        }
    }

    if((wall[x_mypos][y_mypos].west & mask) == NOWALL){
        tmp_priority =GetPriority(x_mypos-1,y_mypos,west);
        uint8_t nextstep = step[x_mypos-1][y_mypos];
        if(nextstep < little){
            little = nextstep;
            *dire = west;
            priority = tmp_priority;
        }else if(nextstep == little){
            if(priority < tmp_priority){
                *dire = west;
                priority = tmp_priority;
            }
        }
    }
    return (4+*dire-dire_mypos)%4;
}

void SearchAdachi(uint8_t gx,uint8_t gy){
    dire_global nextdire;
    switch(GetNextDire(gx,gy,SEARCH_MASK,&nextdire)){
        case front:
            Straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);
            break;
        case right:
            SpinTurn(90,TURN_ACCEL,TURN_SPEED,RIGHT);
            Straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);
            break;
        case left:
            SpinTurn(90,TURN_ACCEL,TURN_SPEED,LEFT);
            Straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);
            break;
        case back:
            SpinTurn(180,TURN_ACCEL,TURN_SPEED,RIGHT);
            Straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);
            break;
    }

    dire_mypos = nextdire;
    
    switch(dire_mypos){
        case north:
            y_mypos++;
            break;
        case south:
            y_mypos--;
            break;
        case east:
            x_mypos++;
            break;
        case west:
            x_mypos--;
            break;
    }

    while((x_mypos != gx) || (y_mypos != gy)){
        SetWall(x_mypos,y_mypos);
        switch(GetNextDire(gx,gy,SEARCH_MASK,&nextdire)){
            case front:
                Straight(FULL_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);
                break;
            case right:
                Straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);
                SpinTurn(90,TURN_ACCEL,TURN_SPEED,RIGHT);
                Straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);
                break;
            case left:
                Straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);
                SpinTurn(90,TURN_ACCEL,TURN_SPEED,LEFT);
                Straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);
                break;
            case back:
                Straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);
                SpinTurn(180,TURN_ACCEL,TURN_SPEED,RIGHT);
                Straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);
                break;
        }
        dire_mypos = nextdire;
    
        switch(dire_mypos){
            case north:
                y_mypos++;
                break;
            case south:
                y_mypos--;
                break;
            case east:
                x_mypos++;
                break;
            case west:
                x_mypos--;
                break;
        }
    }
    SetWall(x_mypos,y_mypos);
    Straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);
}