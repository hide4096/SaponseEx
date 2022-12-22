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
    tgt_angvel = 0.;
    max_spd = _max_spd;
    accel = _accel;
    wallfix_is = DISABLE_MODE;
    runmode = STRAIGHT_MODE;

    led = 0b010;

    if(_end_spd == 0){
        while( (tgt_len - 10. - len) > 1000*(((float)(tgt_spd*tgt_spd) - (float)(_end_spd*_end_spd))/(float)(2.0*accel)) );
        led = 0b101;
        accel = -_accel;
        while(len < tgt_len -1){
            if(tgt_spd <= MIN_SPEED){
                accel = 0;
                tgt_spd = MIN_SPEED;
            }
        }
        accel = 0;
        tgt_spd = 0;
        while (spd >= 0.0);
    }else{
        //加速して巡行する
        while( (tgt_len - 10. - len) > 1000*(((float)(tgt_spd*tgt_spd) - (float)(_end_spd*_end_spd))/(float)(2.0*accel)) );
        led = 0b101;

        //ゴールギリ手前まで減速する
        accel = -_accel;
        while(len < tgt_len){
            //最低速度に達したら定速走行
            if(tgt_spd <= _end_spd){
                    accel = 0.;
            }
        }
        while(spd >= 0);
    }
    accel = 0;
    len = 0.;
}

void SpinTurn(float _deg,float _ang_accel,float _max_angvel,int8_t _dir){
    HAL_Delay(DELAY);
    I_angvel = 0.;
    I_spd = 0.;
    I_error = 0;
    accel = 0;
    tgt_spd = 0;
    tgt_angvel = 0;
    max_spd = 0.;
    turndir = _dir;
    float start_deg = deg;
    float tgt_deg;

    runmode = TURN_MODE;
    led = 0b010;

    if(turndir == LEFT){
        ang_accel = _ang_accel;
        max_angvel = _max_angvel;
        tgt_deg = _deg;
        while((tgt_deg - (deg - start_deg))*M_PI/180. > (tgt_angvel*tgt_angvel/(2.0*ang_accel)) );
    }else if(turndir == RIGHT){
        ang_accel = -_ang_accel;
        max_angvel = -_max_angvel;
        tgt_deg = -_deg;
        while(-(float)(tgt_deg - (deg - start_deg))*M_PI/180. > (float)(tgt_angvel*tgt_angvel/(2.0*-ang_accel)) );
    }

    led = 0b101;

    if(turndir == LEFT){
        ang_accel = -_ang_accel;
        while((deg - start_deg) < tgt_deg){
            if(tgt_angvel < MIN_ANGVEL){
                ang_accel = 0.;
                tgt_angvel = MIN_ANGVEL;
            }
        }

        ang_accel = 0.;
        tgt_angvel = 0.;
    }else if(turndir == RIGHT){
        ang_accel = +_ang_accel;
        while((deg - start_deg) > tgt_deg){
            if(tgt_angvel < MIN_ANGVEL){
                ang_accel = 0.;
                tgt_angvel = -MIN_ANGVEL;
            }
        }

        ang_accel = 0.;
        tgt_angvel = 0.;
    }

    while(angvel >= 2.5 || angvel <= -2.5);

    ang_accel = 0.;
    tgt_angvel = 0.;
    len = 0.;
    HAL_Delay(DELAY);
}