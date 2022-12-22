/* 
###############################
    System Interrupt & Init
###############################
*/
#include"system.h"

static uint8_t mode = 4;
static uint8_t txbuf[64];

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
  }
  else if(htim == &htim7){
    //1kHz
    GetBattVoltage();
    ControlDuty();
    FailSafe();
    timer++;
  }
  else if(htim == &htim10){
    //4kHz
    TrigWallSens();
  }
}

void init(){
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


  //迷路情報を初期化
  InitMaze();

  HAL_TIM_Base_Start_IT(&htim6);  //interrupt 2kHz
  HAL_TIM_Base_Start_IT(&htim7);  //interrupt 1kHz
  HAL_TIM_Base_Start_IT(&htim10);  //interrupt 4kHz

  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcval, 9);
}

void mainmenu(){
  I_angvel = 0;
  angvel = 0;

  if(sensval[FL] + sensval[FR] >= CONFIRM*2){
    Blink(2);
    switch (mode){
      case 1:
        Blink(2);
        SetLED(0b111);
        r_yaw_ref = IMU_SurveyBias(GYROREFTIME);
        deg = 0;
        timer = 0;
        x_mypos = 0;
        y_mypos = 0;
        dire_mypos = north;
        SetLED(0b000);
        SearchAdachi(GOAL_X,GOAL_Y);
        runmode = DISABLE_MODE;
        HAL_Delay(1000);
        while(sensval[FL] + sensval[FR] >= CONFIRM*2);
        break;
      case 2:
        while(1){
          sprintf(txbuf,"%d\t%d\t%d\t%d\r\n",sensval[SL],sensval[FL],sensval[FR],sensval[SSR]);
          HAL_UART_Transmit(&huart6,txbuf,strlen((char*)txbuf),1000);
          HAL_Delay(1000);
        }
        break;
      case 3:
        while(1){
          ITM_SendChar((uint8_t)(sensval[SL]>>4),1);
          ITM_SendChar((uint8_t)(sensval[SSR]>>4),2);
          HAL_Delay(10);
        }
        break;
      case 4:
        r_yaw_ref = IMU_SurveyBias(GYROREFTIME);
        Straight(FULL_SECTION,0,0,0);
        break;
      case 5:
        tvL = tvR = 1.0;
        runmode = TEST_MODE;
        for(int i=0;i<100;i++){
          ITM_SendChar((uint8_t)vbat*100.,1);
          HAL_Delay(10);
        }
        runmode = DISABLE_MODE;
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
  spd = 0.;
  SetLED(mode);

  runmode = DISABLE_MODE;
}