#include"interrupt.h"

static const float alpha = 0.4;
static const float max_integral = 1000;

static float CalcVelocity(){
    static float velocity = 0.0f;

    //エンコーダーから速度を計算する
    static uint16_t rotateR = 0,rotateL = 0;

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

    float length = (diff_rotateR + diff_rotateL) * 0.5f * (M_PI*TIRE_DIAMETER/AS5047P_MAX_ANGLE);

    //IMUと相補フィルターで速度を計算する
    float accel = accelY() / 1000.0f;
    velocity = alpha * (velocity + accel) + (1.0f - alpha) * length;

    //時速はm/s
    return velocity;
}

static float CalcAngularVelocity(){
    return -1. * gyroZ() / 1000.0f;
}

struct targetPID{
    float v;
    float w;
};

static void PID(){
    static struct targetPID integral = {
        .v = 0.0f,
        .w = 0.0f
    };
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

    float right = error.v * straight.Kp + integral.v * straight.Ki + (error.v - past_error.v) * straight.Kd;
    right += error.w * turn.Kp + integral.w * turn.Ki + (error.w - past_error.w) * turn.Kd;
    float left = error.v * straight.Kp + integral.v * straight.Ki + (error.v - past_error.v) * straight.Kd;
    left -= error.w * turn.Kp + integral.w * turn.Ki + (error.w - past_error.w) * turn.Kd;

    past_error = error;

    //モーターに出力する
    right /= 4.0f;
    left /= 4.0f;
    Motors_set(&motors, right, left);
}

void interrupt_init(){
    HAL_TIM_Base_Start_IT(&htim6);
}

void interrupt_1ms(){
    mouse.v = CalcVelocity();
    mouse.w = CalcAngularVelocity();
    PID();
}