/* 
###############################
    System Interrupt & Init
###############################
*/
#include"system.h"

static uint8_t mode = 1;

unsigned int timer = 0;

void DoPanic(){
  runmode = DISABLE_MODE;
  HAL_TIM_Base_Stop_IT(&htim6);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,0);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,0);
  printf("Soiya!!\r\n");
  SetLED(0b000);
  while(1);
}

/* HM-SterterKitの場合
  モータ制御  1kHz
  AD変換      4kHz(一度に見るのは1個なので実質1KHz)
  センサ取得  2kHz
*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
  if(htim == &htim6){
    //2kHz
    GetSpeed();
    GetYawDeg();
    TrigWallSens();
  }
  else if(htim == &htim7){
    //1kHz
    GetBattVoltage();
    ControlDuty();
    FailSafe();
    timer++;
  }
}

void init(){

  InitADCDMA();

  //InitEnc
  uint8_t errcnt = 0;
  AS5047P_Init(&encL, 0);
  AS5047P_Init(&encR, 1);
  while(AS5047P_ErrorPending(&encR) || AS5047P_ErrorPending(&encL)){
    errcnt++;
    if(errcnt >= MAXINITERR) DoPanic();
    AS5047P_Init(&encL, 0);
    AS5047P_Init(&encR, 1);
    HAL_Delay(100);
  }
  AS5047P_SetZeroPosition(&encL);
  AS5047P_SetZeroPosition(&encR);
  errcnt = 0;

  //InitIMU
  if(IMU_init(&hspi2,CS_IMU_GPIO_Port,CS_IMU_Pin) < 0) DoPanic();
  r_yaw_ref = IMU_SurveyBias(GYROREFTIME);

  //Motor
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
  HAL_GPIO_WritePin(FAN_GPIO_Port,FAN_Pin,0);
  runmode = DISABLE_MODE;

  //LED
  SetLED(0b000);

  HAL_TIM_Base_Start_IT(&htim6);  //interrupt 2kHz
  HAL_TIM_Base_Start_IT(&htim7);  //interrupt 1kHz
}

void mainmenu(){
  I_angvel = 0;
  angvel = 0;

  if(sensval[SL] + sensval[SR] >= CONFIRM*2){
    Blink(2);
    switch (mode){
      case 1:
        Blink(2);
        SetLED(0b111);
        r_yaw_ref = IMU_SurveyBias(GYROREFTIME);
        deg = 0;
        timer = 0;
        SetLED(0b000);
        straight(HALF_SECTION*2,SEARCH_ACCEL,SEARCH_SPEED,0);
        turn(90,TURN_ACCEL,TURN_SPEED,RIGHT);
        runmode = DISABLE_MODE;
        break;
      case 2:
        while(1){
          printf("%d\t%d\t%d\t%d\r\n",sensval[SL],sensval[FL],sensval[FR],sensval[SR]);
        }
        break;
      case 3:
        while(1){
          printf("%.3f\t%.3f\r\n",deg,angvel);
        }
        break;
      default:
        DoPanic();
        break;
    }
  }

  //モード選択
  if(spd > 0.1){
    if(mode < 0b111){
      mode++;
      Blink(5);
    }
  }else if(spd < -0.1){
    if(mode > 0b001){
      mode--;
      Blink(5);
    }
  }
  SetLED(mode);

  runmode = DISABLE_MODE;
}