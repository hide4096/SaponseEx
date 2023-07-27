#include"run.h"
#include"main.h"

void Straight(float tgt_len,float _accel,float _max_spd,float _end_spd){
    resetIntegral();
    target.v = 0.0f;
    target.w = 0.0f;
    target_HM.max_w = 0.;
    target_HM.max_v = _max_spd;
    target_HM.a = _accel;

    if(_end_spd == 0){
        while( ((tgt_len-30) - target_HM.len) > 1000*(((float)(target.v*target.v) - (float)(_end_spd*_end_spd))/(float)(2.0*target_HM.a)) );
        target_HM.a = -_accel;
        while(target_HM.len < tgt_len -1){
            if(target.v <= MIN_SPEED){
                target_HM.a = 0;
                target.v = MIN_SPEED;
            }
        }
        target_HM.a = 0;
        target.v = 0;
        while (mouse.v >= 0.0);
    }else{
        //加速して巡行する
        while( ((tgt_len-30) - target_HM.len) > 1000*(((float)(target.v*target.v) - (float)(_end_spd*_end_spd))/(float)(2.0*target_HM.a)) );

        //ゴールギリ手前まで減速する
        target_HM.a = -_accel;
        while(target_HM.len < tgt_len){
            //最低速度に達したら定速走行
            if(target.v <= _end_spd){
                    target_HM.a = 0.;
                    target.v = _end_spd;
            }
        }
    }
    target_HM.a = 0;
    target_HM.len = 0.;
}

void SpinTurn(float _deg,float _ang_accel,float _max_angvel,int8_t _dir){
    HAL_Delay(DELAY);
    resetIntegral();
    target_HM.a = 0;
    target.v = 0;
    target.w = 0;
    target_HM.max_v = 0.;
    target_HM.turndir = _dir;
    target_HM.deg = 0.;
    float tgt_deg;

    if(target_HM.turndir == LEFT){
        target_HM.ang_a = _ang_accel;
        target_HM.max_w = _max_angvel;
        tgt_deg = _deg;
        while((float)(tgt_deg - target_HM.deg)*M_PI/180. > (float)(target.w*target.w/(2.0*target_HM.ang_a)));
    }else if(target_HM.turndir == RIGHT){
        target_HM.ang_a = _ang_accel;
        target_HM.max_w = _max_angvel;
        tgt_deg = -_deg;
        while(-(float)(tgt_deg - target_HM.deg)*M_PI/180. > (float)(target.w*target.w/(2.0*-target_HM.ang_a)));
    }

    if(target_HM.turndir == LEFT){
        target_HM.ang_a = -_ang_accel;
        while(target_HM.deg < tgt_deg){
            if(target.w < MIN_ANGVEL){
                target_HM.ang_a = 0.;
                target.w = MIN_ANGVEL;
            }
        }

        target_HM.ang_a = 0.;
        target.w = 0.;
    }else if(target_HM.turndir == RIGHT){
        target_HM.ang_a = +_ang_accel;
        while(target_HM.deg > tgt_deg){
            if(target.w > -MIN_ANGVEL){
                target_HM.ang_a = 0.;
                target.w = -MIN_ANGVEL;
            }
        }

        target_HM.ang_a = 0.;
        target.w = 0.;
    }

    while(target.w >= 2.5 || target.w <= -2.5);

    target_HM.ang_a = 0.;
    target.w = 0.;
    target_HM.len = 0.;
    HAL_Delay(DELAY);
}