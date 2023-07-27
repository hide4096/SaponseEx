#include"interrupt.h"
#include"main.h"
#include"run.h"

static const float alpha = 0.6;
static const float max_integral = 1000;

struct save_data save[LOGGING_SIZE];

uint64_t count;

static float CalcVelocity(){
    static uint16_t rotateR=0,rotateL=0;
    static float velocity = 0.0f;

    //エンコーダーから速度を計算する

    uint16_t past_rotateR = rotateR;
    uint16_t past_rotateL = rotateL;
    rotateR = readAngle(&encR);
    rotateL = readAngle(&encL);

    int diff_rotateR = rotateR - past_rotateR;
    int diff_rotateL = rotateL - past_rotateL;

    if(diff_rotateR > AS5047P_MAX_ANGLE/2) diff_rotateR -= AS5047P_MAX_ANGLE;
    else if(diff_rotateR < -AS5047P_MAX_ANGLE/2) diff_rotateR += AS5047P_MAX_ANGLE;
    if(diff_rotateL > AS5047P_MAX_ANGLE/2) diff_rotateL -= AS5047P_MAX_ANGLE;
    else if(diff_rotateL < -AS5047P_MAX_ANGLE/2) diff_rotateL += AS5047P_MAX_ANGLE;

    float length = (diff_rotateR + diff_rotateL) * 0.5f * (M_PI*sapoex.diam/AS5047P_MAX_ANGLE);

    //IMUと相補フィルターで速度を計算する
    float accel = accelY() / 1000.0f;
    velocity = alpha * (velocity + accel) + (1.0f - alpha) * length;

    //単位はm/s
    return velocity;
}

static float CalcAngularVelocity(){
    return -1. * gyroZ() / 1000.0f;
}

struct targetPID{
    float v;
    float w;
};

static struct targetPID integral = {
    .v = 0.0f,
    .w = 0.0f
};

static void PID_FF(){
    static float past_target_v = 0.0f;

    //フィードフォワード
    float accel_target = (target.v-past_target_v)*1000.;
    float right_ff = 0.0f;
    float left_ff = 0.0f;
    if(accel_target > 0.){
        right_ff = accel_target*ff.ka;
        left_ff = accel_target*ff.ka;
    }else{
        right_ff = accel_target*ff.ka2 + mouse.v*ff.kv;
        left_ff = accel_target*ff.ka2 + mouse.v*ff.kv;
    }

    past_target_v = target.v;

    //フィードバック
    static struct targetPID past_error = {
        .v = 0.0f,
        .w = 0.0f
    };

    struct targetPID error = {
        .v = target.v - mouse.v,
        .w = target.w - mouse.w
    };

    integral.v += error.v;
    integral.w += error.w;

    if(integral.v > max_integral) integral.v = max_integral;
    else if(integral.v < -max_integral) integral.v = -max_integral;
    if(integral.w > max_integral) integral.w = max_integral;
    else if(integral.w < -max_integral) integral.w = -max_integral;

    float right_fb = error.v * straight.Kp + integral.v * straight.Ki + (error.v - past_error.v) * straight.Kd;
    right_fb -= error.w * turn.Kp + integral.w * turn.Ki + (error.w - past_error.w) * turn.Kd;
    float left_fb = error.v * straight.Kp + integral.v * straight.Ki + (error.v - past_error.v) * straight.Kd;
    left_fb += error.w * turn.Kp + integral.w * turn.Ki + (error.w - past_error.w) * turn.Kd;

    past_error = error;

    //モーターに出力する
    Motors_set(&motors, (right_fb + right_ff)/sensor.vbat, (left_fb + left_ff)/sensor.vbat);

    if(use_logging){
        save[count].fbR = right_fb / sensor.vbat;
        save[count].fbL = left_fb / sensor.vbat;
        save[count].ffR = right_ff / sensor.vbat;
        save[count].ffL = left_ff / sensor.vbat;
    }
}

void resetIntegral(){
    integral.v = 0.0f;
    integral.w = 0.0f;
}

void control_loop_start(){
    HAL_TIM_Base_Start_IT(&htim6);
    count = 0;
    resetIntegral();
}

void control_loop_stop(){
    HAL_TIM_Base_Stop_IT(&htim6);
}

void analog_sensing_start(){
    HAL_TIM_Base_Start_IT(&htim7);
}

void analog_sensing_stop(){
    HAL_TIM_Base_Stop_IT(&htim7);
}

uint8_t use_HMmode = FALSE;

void interrupt_1ms(){
    mouse.v = CalcVelocity();
    mouse.w = CalcAngularVelocity();
    if(use_HMmode){
        target_HM.len += mouse.v*0.001f;
        target_HM.deg += mouse.w*0.001f;
        if(target_HM.mode == STRAIGHT_MODE || target_HM.mode == TURN_MODE){
            target.v = target_HM.a * 1000.f;
            if(target.v > target_HM.max_v) target.v = target_HM.max_v;
        }

        if(target_HM.mode == TURN_MODE){
            target.w = target_HM.ang_a * 1000.f;
            if(target_HM.turndir == LEFT){
                if(target.w > target_HM.max_w) target.w = target_HM.max_w;
            }else if(target_HM.turndir == RIGHT){
                if(target.w < target_HM.max_w) target.w = target_HM.max_w;
            }
        }
    }
    PID_FF();
    if(use_logging){
        if(count >= LOGGING_SIZE){
            control_loop_stop();
            return;
        }
        save[count].mouse = mouse;
        save[count].target = target;
        save[count].count = count;
    }
    count++;
}

void interrupt_500us(){
    TrigWallSens();
}

struct save_data* savedataHandle(){
    return save;
}