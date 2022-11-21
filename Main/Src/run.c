/* 
####################
    Run and Turn
####################
*/
#include"run.h"

extern float I_angvel,I_spd;
extern float spd,deg,accel,max_spd,len;
extern uint8_t runmode,wallfix_is;

void straight(float tgt_len,float _accel,float _max_spd,float _end_spd){
    I_angvel = 0.;
    I_spd = 0.;
    I_error = 0;
    len = 0.;
    tgt_angvel = 0.;
    max_spd = _max_spd;
    accel = _accel;
    wallfix_is = DISABLE_MODE;
    runmode = STRAIGHT_MODE;

    //加速して巡行する
    
    while( (tgt_len - len) > 1000*(((tgt_spd*tgt_spd) - (_end_spd*_end_spd) - 2.0*(tgt_spd*_end_spd))/(2.0*accel)) ){
        HAL_Delay(1);   //ここ消すと無限に走っちゃう（RTOSかよ）
    }

    //ゴールギリ手前まで減速する
    accel = -_accel;
    while(tgt_len < len - 1){
        //最低速度に達したら定速走行
        if(tgt_spd <= MIN_SPEED){
            accel = 0.;
            tgt_spd = MIN_SPEED;
        }
    }

    tgt_spd = _end_spd;
    while(spd >= _end_spd);

    accel = 0;
}

void turn(){
}