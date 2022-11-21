/* 
#####################################
    Motor Control & Encoder & IMU
#####################################
*/

#include"motor.h"

static int16_t b_encR_val=0,b_encL_val=0;
static float spdR = 0,spdL=0;
static float b_spdR = 0,b_spdL=0;
static float r_yaw = 0,r_yaw_new = 0,r_b_yaw;
static float before_spd=0.;
static float before_angvel=0.;

uint8_t runmode=DISABLE_MODE;
uint8_t wallfix_is = DISABLE_MODE;
uint8_t turndir = LEFT;
float spd,deg,len;
float accel = 0.,ang_accel = 0.;
float max_spd = 0.;
float max_angvel = 0.;
float angvel,r_yaw_ref;
float tgt_spd=0.,tgt_angvel=0.;

float I_spd = 0.;
float I_angvel = 0.;
int I_error = 0.;


AS5047P_Instance encR;
AS5047P_Instance encL;

void SetDutyRatio(uint16_t motL,uint16_t motR,uint8_t motR_isCW,uint8_t motL_isCW){
  if(runmode != DISABLE_MODE){
    if(motR > MTPERIOD) motR = MTPERIOD;
    if(motL > MTPERIOD) motL = MTPERIOD;

    if(motL_isCW){
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,motL);
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,0);
    }else{
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,0);
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,motL);  
    }

    if(motR_isCW){
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,0);
      __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,motR);
    }else{
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,motR);
      __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,0);
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

void ControlDuty(){
  float vR = 0.,vL = 0.;

  //速度生成
  tgt_spd += accel/1000.;
  if(tgt_spd > max_spd) tgt_spd = max_spd;
  if(runmode == TURN_MODE){
    tgt_angvel += ang_accel/1000.0;
    if(tgt_angvel*turndir > max_angvel) tgt_angvel = max_angvel*turndir;
  }else{
      tgt_angvel = 0.;
  }

  //壁制御
  if(runmode == STRAIGHT_MODE){
    if(wallfix_is == ENABLE_MODE){
      //壁あるか確認
      uint8_t fl_wall_is = 0,fr_wall_is = 0;
      if(sensval[FL] >= WALL_TH_FL) fl_wall_is = 1;
      if(sensval[FR] >= WALL_TH_FR) fr_wall_is = 1;

      /*
        壁との距離から目標コースとのずれを測る
        壁の有無に応じて切り替えるよ
      */
      int error = 0;
      if(fl_wall_is && fr_wall_is){
        error = (sensval[FL] - REF_FL) - (sensval[FR] - REF_FR); 
      }else if(fl_wall_is){
        error = (sensval[FL] - REF_FL)*2;
      }else if(fr_wall_is){
        error = 0. - (sensval[FR] - REF_FR)*2;
      }

      //PIDして目標角速度に出力
      I_error += error;
      if(I_error > WALLERROR_I_MAX) I_error = WALLERROR_I_MAX;
      if(I_error < -WALLERROR_I_MAX) I_error = -WALLERROR_I_MAX;
      tgt_angvel = error * WALL_KP + I_error * WALL_KI;
    }else{
      tgt_angvel = 0.;
      I_error = 0.;
    }
  }

  //速度フィードバック
  float diff_spd = tgt_spd - spd;
  float v_spd = diff_spd*SPD_KP+I_spd*SPD_KI+(before_spd-spd)*SPD_KD;
  before_spd = spd;
  I_spd+=diff_spd;
  if(I_spd > SPD_I_MAX) I_spd = SPD_I_MAX;
  else if(I_spd < -SPD_I_MAX) I_spd = -SPD_I_MAX;

  //角速度フィードバック
  float diff_angvel = tgt_angvel - angvel;
  float v_angvel = diff_angvel*ANGVEL_KP+I_angvel*ANGVEL_KI+(before_angvel-angvel)*ANGVEL_KD;
  before_angvel = angvel;
  I_angvel+=diff_angvel;
  if(I_angvel > ANGVEL_I_MAX) I_angvel = ANGVEL_I_MAX;
  else if(I_angvel < -ANGVEL_I_MAX) I_angvel = -ANGVEL_I_MAX;

  //合算
  vR = v_spd + v_angvel;
  vL = v_spd - v_angvel;

  //電圧をデューティ比に変換
  uint8_t motL_isCW = 0;
  uint8_t motR_isCW = 1;
  if(vL < 0){
    motL_isCW = 1;
    vL*=-1.;
  }
  if(vR < 0){
    motR_isCW = 0;
    vR*=-1.;
  }
  float dutyR = vR/vbat;
  float dutyL = vL/vbat;

  //デューティ比を設定
  SetDutyRatio(MTPERIOD*dutyL,MTPERIOD*dutyR,motL_isCW,motR_isCW);
}

void FailSafe(){
  if(spdL < FAILSAFE && spdL > -FAILSAFE) return;
  if(spdR < FAILSAFE && spdR > -FAILSAFE) return;
  if(vbat > LOWVOLT) return;
  SetLED(0b000);
  while(1);
}

void GetYawDeg(){
  r_yaw_new = gyroZ() - r_yaw_ref;
  
  r_b_yaw = r_yaw;
  r_yaw = r_b_yaw * IMULPF + r_yaw_new * (1.0 - IMULPF);

  angvel = r_yaw*-1.;

  deg += angvel * DELTA_T;
}