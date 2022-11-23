/* 
####################
    Run and Turn
####################
*/
#include"run.h"

void Straight(float tgt_len,float _accel,float _max_spd,float _end_spd){
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
    SetLED(0b010);

    while( (tgt_len - len) > 1000*(((tgt_spd*tgt_spd) - (_end_spd*_end_spd) - 2.0*(tgt_spd*_end_spd))/(2.0*accel)) ){
        //HAL_Delay(1);   //ここ消すと無限に走っちゃう（RTOSかよ）
        printf("%f\r\n",tgt_spd);
    }


    //ゴールギリ手前まで減速する
    accel = -_accel;
    while(len < tgt_len - 1){
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

void SpinTurn(float _deg,float _ang_accel,float _max_angvel,uint8_t _dir){
    I_angvel = 0.;
    I_spd = 0.;
    I_error = 0;
    len = 0.;
    accel = 0;
    tgt_spd = 0;
    tgt_angvel = 0;
    max_spd = 0.;
    turndir = _dir;
    float start_deg = deg;

    runmode = TURN_MODE;

    ang_accel = _ang_accel * turndir;
    max_angvel = _max_angvel * turndir;
    float tgt_deg = _deg * turndir;

    while( (float)(tgt_deg - (deg - start_deg))*turndir > (tgt_angvel*tgt_angvel/ang_accel)*turndir){
        HAL_Delay(1);
    }

    SetLED(0b101);

    ang_accel*=-1;
    while(tgt_deg*turndir > (deg-start_deg)*turndir){
        if(tgt_angvel*turndir < MIN_ANGVEL){
            ang_accel = 0.;
            tgt_angvel = MIN_ANGVEL * turndir;
        }
    }

    ang_accel = 0.;
    tgt_angvel = 0.;
}