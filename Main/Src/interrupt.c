#include"interrupt.h"

static const float alpha = 0.4;
static const float max_integral = 1000;

static float rpmR = 0.0f,rpmL = 0.0f;

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

    rpmL = ((float)diff_rotateL  / AS5047P_MAX_ANGLE) * 1000.*60.*-1.;
    rpmR = ((float)diff_rotateR  / AS5047P_MAX_ANGLE) * 1000.*60.*-1.;


    float length = (diff_rotateR + diff_rotateL) * 0.5f * (M_PI*sapoex.diam/AS5047P_MAX_ANGLE);

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

static void PID_FF(){
    //フィードフォワード
    float accel_target = (target.v-mouse.v)/1000.;
    float torque = ((accel_target * accel_target / 4. * (sapoex.diam / 2)) / sapoex.gear_ratio);
    float right_ff = (torque / sapoex.kT) * sapoex.r + sapoex.ke * rpmR * sapoex.gear_ratio;
    float left_ff = (torque / sapoex.kT) * sapoex.r + sapoex.ke * rpmL * sapoex.gear_ratio;

    //フィードバック
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

    float right_fb = error.v * straight.Kp + integral.v * straight.Ki + (error.v - past_error.v) * straight.Kd;
    right_fb -= error.w * turn.Kp + integral.w * turn.Ki + (error.w - past_error.w) * turn.Kd;
    float left_fb = error.v * straight.Kp + integral.v * straight.Ki + (error.v - past_error.v) * straight.Kd;
    left_fb += error.w * turn.Kp + integral.w * turn.Ki + (error.w - past_error.w) * turn.Kd;

    past_error = error;

    //モーターに出力する
    Motors_set(&motors, (right_fb + right_ff)/sensor.vbat, (left_fb + left_ff)/sensor.vbat);
}

void interrupt_init(){
    HAL_TIM_Base_Start_IT(&htim6);
    HAL_TIM_Base_Start_IT(&htim7);
}

void interrupt_1ms(){
    mouse.v = CalcVelocity();
    mouse.w = CalcAngularVelocity();
    PID_FF();
}

void interrupt_500us(){
    TrigWallSens();
}