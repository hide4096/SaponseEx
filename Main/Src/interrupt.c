#include"interrupt.h"

static const float alpha = 0.8;
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
    if(accel_target > 0.){
        float right_ff = accel_target*ff.ka;
        float left_ff = accel_target*ff.ka;
    }else{
        float right_ff = accel_target*ff.ka + mouse.v*ff.kv;
        float left_ff = accel_target*ff.ka + mouse.v*ff.kv;
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
        save[count].accel = accel_target;
    }
}

void control_loop_start(){
    HAL_TIM_Base_Start_IT(&htim6);
    count = 0;
    integral.v = 0.0f;
    integral.w = 0.0f;
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

void interrupt_1ms(){
    mouse.v = CalcVelocity();
    mouse.w = CalcAngularVelocity();
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