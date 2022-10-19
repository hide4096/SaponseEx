/* 
#####################################
    Motor Control & Encoder & IMU
#####################################
*/

#include"motor.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern AS5047P_Instance encR;
extern AS5047P_Instance encL;

static int16_t b_encR_val=0,b_encL_val=0;
static float spdR = 0,spdL=0;
static float b_spdR = 0,b_spdL=0;
static float b_angvel = 0;
static float r_yaw = 0,r_yaw_new = 0,r_b_yaw;

uint8_t motpower=0;
float spd,deg;
float tgt_spd,tgt_deg;
float angvel,r_yaw_ref;

void SetDutyRatio(int16_t motL,int16_t motR,int16_t motF){
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

    if(motF > MTPERIOD) motF = MTPERIOD;
    __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,2000);
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

  spdR = n_spdR * ENCLPF + b_spdR * (1.0 - ENCLPF);
  spdL = n_spdL * ENCLPF + b_spdL * (1.0 - ENCLPF);

  spd = (spdR + spdL) / 2.0;  //
  b_encR_val = encR_val;
  b_encL_val = encL_val;
}

void ControlDuty(){
  float dutyR = 0.0,dutyL = 0.0;
  dutyR = (tgt_spd - spd)*6.0 + (tgt_deg - deg)*0.015;
  dutyL = (tgt_spd - spd)*6.0 - (tgt_deg - deg)*0.015;

  SetDutyRatio(MTPERIOD*dutyL,MTPERIOD*dutyR,0);
}

void GetYawDeg(){
  r_yaw_new = gyroZ() - r_yaw_ref;
  r_b_yaw = r_yaw;
  r_yaw = r_yaw_new * IMULPF + r_b_yaw * (1.0 - IMULPF);
  b_angvel = angvel;
  angvel = (2000.0*r_yaw/32767.0)*PI/180.0;
  deg += 2.0*r_yaw/32767.0;
}