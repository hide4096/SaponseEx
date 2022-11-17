/* 
#####################################
    Motor Control & Encoder & IMU
#####################################
*/

#include"motor.h"
#include"analog.h"

static int16_t b_encR_val=0,b_encL_val=0;
static float spdR = 0,spdL=0;
static float b_spdR = 0,b_spdL=0;
static float r_yaw = 0,r_yaw_new = 0,r_b_yaw;

uint8_t motpower=0;
float spd,deg,len;
float tgt_spd,tgt_angvel;
float angvel,r_yaw_ref;

AS5047P_Instance encR;
AS5047P_Instance encL;

void SetDutyRatio(int16_t motL,int16_t motR){
  if(motpower){
    if(motR > 0){
      if(motR > MTPERIOD) motR = MTPERIOD;
     __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,motR);
     __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,0);
    }else{
      motR*=-1;
      if(motR > MTPERIOD) motR = MTPERIOD;
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,0);
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,motR);  
    }

    if(motL > 0){
      if(motL > MTPERIOD) motL = MTPERIOD;
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,motL);
      __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,0);
    }else{
      motL*=-1;
      if(motL > MTPERIOD) motL = MTPERIOD;
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,0);
      __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,motL);
    }
  }else{
    __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,0);
    __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,0);
    __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,0);
    __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,0);
  }
}

void GetSpeed(){
  int16_t encR_val,encL_val;

  //エンコーダ読む
  encR_val = AS5047P_ReadPosition(&encR, AS5047P_OPT_ENABLED);
  encL_val = AS5047P_ReadPosition(&encL, AS5047P_OPT_ENABLED);
	if(AS5047P_ErrorPending(&encR)) AS5047P_ErrorAck(&encR);
	if(AS5047P_ErrorPending(&encL)) AS5047P_ErrorAck(&encL);

  //差分とる
  int16_t d_encR_val = encR_val - b_encR_val;
  int16_t d_encL_val = b_encL_val -  encL_val;
  b_encR_val = encR_val;
  b_encL_val = encL_val;

  //0と16383とかまたいだ時の処理
  //秒速98.2m/s超えるとバグる（笑）
  if(d_encR_val > ENC_HALF)   d_encR_val -= ENC_MAX-1;
  if(d_encR_val < -ENC_HALF)  d_encR_val += ENC_MAX-1;
  if(d_encL_val > ENC_HALF)   d_encL_val -= ENC_MAX-1;
  if(d_encL_val < -ENC_HALF)  d_encL_val += ENC_MAX-1;

  //進んだ距離計算する
  float lenR = (float)d_encR_val * (float)PPMM;
  float lenL = (float)d_encL_val * (float)PPMM;
  //走行距離に入れる
  len += (lenR + lenL) / 2.0;

  //進んだ距離から速度計算する
  float n_spdR = (lenR / 1000.) / DELTA_T;
  float n_spdL = (lenL / 1000.) / DELTA_T;

  b_spdR = spdR;
  b_spdL = spdL;
  spdR = b_spdR * ENCLPF + n_spdR * (1.0 - ENCLPF);
  spdL = b_spdL * ENCLPF + n_spdL * (1.0 - ENCLPF);
  

  //機体全体の速度を計算する
  spd = (spdR + spdL) / 2.0;
}

static float before_spd=0.;
float I_spd = 0.;
static float before_angvel=0.;
float I_angvel = 0.;

void ControlDuty(){
  float dutyR = 0.,dutyL = 0.;

  //float diff_spd = tgt_spd - spd;
  float diff_spd = (tgt_spd - spd) + (5000 - (sensval[1] + sensval[2]))*0.0002;
  float duty_spd = diff_spd*SPD_KP+I_spd*SPD_KI+(before_spd-spd)*SPD_KD;
  before_spd = spd;
  I_spd+=diff_spd;

  if(I_spd > SPD_I_MAX) I_spd = SPD_I_MAX;
  else if(I_spd < -SPD_I_MAX) I_spd = -SPD_I_MAX;


  //float diff_angvel = (tgt_angvel - angvel) + (sensval[2] - sensval[1])*WALL_KP;
  float diff_angvel = (tgt_angvel - angvel) + (sensval[0] - sensval[2])*WALL_KP;
  //float diff_angvel = (tgt_angvel - angvel);
  float duty_angvel = diff_angvel*ANGVEL_KP+I_angvel*ANGVEL_KI+(before_angvel-angvel)*ANGVEL_KD;
  before_angvel = angvel;
  I_angvel+=diff_angvel;

  if(I_angvel > ANGVEL_I_MAX) I_angvel = ANGVEL_I_MAX;
  else if(I_angvel < -ANGVEL_I_MAX) I_angvel = -ANGVEL_I_MAX;
  
  dutyR = duty_spd - duty_angvel;
  dutyL = duty_spd + duty_angvel;

  SetDutyRatio(MTPERIOD*dutyL,MTPERIOD*dutyR);
}

void FailSafe(){
  if(spdL < FAILSAFE && spdL > -FAILSAFE) return;
  if(spdR < FAILSAFE && spdR > -FAILSAFE) return;
  if(vbat > LOWVOLT) return;
  while(1) DoPanic();
}

void GetYawDeg(){
  r_yaw_new = gyroZ() - r_yaw_ref;
  
  r_b_yaw = r_yaw;
  r_yaw = r_b_yaw * IMULPF + r_yaw_new * (1.0 - IMULPF);

  angvel = r_yaw;

  deg += r_yaw * DELTA_T;
}