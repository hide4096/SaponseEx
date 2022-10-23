/* 
#####################################
    Motor Control & Encoder & IMU
#####################################
*/

#include"motor.h"

static int16_t b_encR_val=0,b_encL_val=0;
static float spdR = 0,spdL=0;
static float b_spdR = 0,b_spdL=0;
static float b_angvel = 0;
static float r_yaw = 0,r_yaw_new = 0,r_b_yaw;

uint8_t motpower=0;
float spd,deg;
float tgt_spd,tgt_deg;
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
  encR_val = AS5047P_ReadPosition(&encR, AS5047P_OPT_ENABLED);
  encL_val = AS5047P_ReadPosition(&encL, AS5047P_OPT_ENABLED);
	if(AS5047P_ErrorPending(&encR)) AS5047P_ErrorAck(&encR);
	if(AS5047P_ErrorPending(&encL)) AS5047P_ErrorAck(&encL);
  int16_t d_encR_val = encR_val - b_encR_val;
  int16_t d_encL_val = b_encL_val -  encL_val;
  if((d_encR_val > ENC_HALF || d_encR_val < -ENC_HALF) && b_encR_val > ENC_HALF){
    d_encR_val = ((ENC_MAX - 1) - b_encR_val) + encR_val;
  }
  else if((d_encR_val > ENC_HALF || d_encR_val < -ENC_HALF) && b_encR_val <= ENC_HALF){
    d_encR_val = (b_encR_val + ((ENC_MAX -1)-encR_val));
  }
  if((d_encL_val > ENC_HALF || d_encL_val < -ENC_HALF) && b_encL_val > ENC_HALF){
    d_encL_val = ((ENC_MAX - 1) - b_encL_val) + encL_val;
  }
  else if((d_encL_val > ENC_HALF || d_encL_val < -ENC_HALF) && b_encL_val <= ENC_HALF){
    d_encL_val = (b_encL_val + ((ENC_MAX -1)-encL_val));
  }

  float n_spdR = (float)d_encR_val * (float)PPMM;
  float n_spdL = (float)d_encL_val * (float)PPMM;
  b_spdR = spdR;
  b_spdL = spdL;

  spdR = b_spdR * ENCLPF + n_spdR * (1.0 - ENCLPF);
  spdL = b_spdL * ENCLPF + n_spdL * (1.0 - ENCLPF);

  spd = (spdR + spdL) / 2.0;  //
  b_encR_val = encR_val;
  b_encL_val = encL_val;
}

static float before_spd=0.;
float I_spd = 0.;
static float before_deg=0.;
float I_deg = 0.;

void ControlDuty(){
  float dutyR = 0.,dutyL = 0.;

  float diff_spd = tgt_spd - spd;
  float duty_spd = diff_spd*SPD_KP+I_spd*SPD_KI+(before_spd-spd)*SPD_KD;
  before_spd = spd;
  I_spd+=diff_spd;

  if(I_spd > SPD_I_MAX) I_spd = SPD_I_MAX;
  else if(I_spd < -SPD_I_MAX) I_spd = -SPD_I_MAX;

  float diff_deg = tgt_deg - deg;
  float duty_deg = diff_deg*DEG_KP+I_deg*DEG_KI+(before_deg-deg)*DEG_KD;
  before_deg = deg;
  I_deg+=diff_deg;

  if(I_deg > DEG_I_MAX) I_deg = DEG_I_MAX;
  else if(I_deg < -DEG_I_MAX) I_deg = -DEG_I_MAX;
  
  dutyR = duty_spd - duty_deg;
  dutyL = duty_spd + duty_deg;

  SetDutyRatio(MTPERIOD*dutyL,MTPERIOD*dutyR);
}

void FailSafe(){
  if(spdL > FAILSAFE || spdL < -FAILSAFE) motpower = 0;
  if(spdR > FAILSAFE || spdR < -FAILSAFE) motpower = 0;
}

void GetYawDeg(){
  r_yaw_new = gyroZ() - r_yaw_ref;
  r_b_yaw = r_yaw;
  r_yaw = r_b_yaw * IMULPF + r_yaw_new * (1.0 - IMULPF);
  b_angvel = angvel;
  angvel = r_yaw;
  deg += r_yaw/1000.;
}