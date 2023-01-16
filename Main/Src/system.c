/* 
###############################
    System Interrupt & Init
###############################
*/
#include"system.h"

static uint8_t mode = 3;
static uint8_t txbuf[64];

unsigned int timer = 0;
volatile uint32_t cnt = 0;
volatile int32_t save[4][4096];

void DoPanic(){
  runmode = DISABLE_MODE;
  HAL_TIM_Base_Stop_IT(&htim6);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,0);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,0);
  printf("Soiya!!\r\n");
  led = 0b000;
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
    //FailSafe();
    SetLED();
    timer++;

    ITM_SendChar(sensval[0]/16,1);
    ITM_SendChar(sensval[1]/16,2);
    ITM_SendChar(sensval[2]/16,3);
    ITM_SendChar(sensval[3]/16,4);
    //printf("%d,%d\r\n",d_encL_val,d_encR_val);
  }
  else if(htim == &htim3){
    //20kHz
    FetchWallSens();
  }
  else if(htim == &htim11){
      save[0][cnt] = (int32_t)(spd*100000);
      save[1][cnt] = (int32_t)(d_encL_val);
      save[2][cnt] = (int32_t)(encL_val);
      save[3][cnt] = (int32_t)(0);
      cnt++;
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
  HAL_Delay(500);
  r_yaw_ref = IMU_SurveyBias(GYROREFTIME);

  //Motor
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
  HAL_GPIO_WritePin(FAN_GPIO_Port,FAN_Pin,0);
  runmode = DISABLE_MODE;

  //LED
  led = 0b000;


  //迷路情報を初期化
  InitMaze();

  //ADC有効化
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcval, 5);

  //割り込みを有効化
  HAL_TIM_Base_Start_IT(&htim3);  //interrupt 20kHz
  HAL_TIM_Base_Start_IT(&htim6);  //interrupt 2kHz
  HAL_TIM_Base_Start_IT(&htim7);  //interrupt 1kHz

  //Compare有効化
  HAL_TIM_OC_Start(&htim3,TIM_CHANNEL_1);
  HAL_TIM_OC_Start(&htim3,TIM_CHANNEL_2);
}

void mainmenu(){
  I_angvel = 0;
  angvel = 0;

  if(sensval[FL] + sensval[FR] >= CONFIRM*2){
    Blink(2);
    switch (mode){
      case 1:
        Blink(2);
        led = 0b111;
        r_yaw_ref = IMU_SurveyBias(GYROREFTIME);
        deg = 0;
        timer = 0;
        x_mypos = 0;
        y_mypos = 0;
        dire_mypos = north;
        led = 0b000;
        SearchAdachi(GOAL_X,GOAL_Y);
        runmode = DISABLE_MODE;
        HAL_Delay(1000);
        while(sensval[FL] + sensval[FR] >= CONFIRM*2);
        break;
      case 2:
        r_yaw_ref = IMU_SurveyBias(GYROREFTIME);
        deg = 0;
        while(1){
          sprintf((char*)txbuf,"%.3f\r\n",deg);
          printf("%s",txbuf);
          HAL_UART_Transmit(&huart6,txbuf,strlen((char*)txbuf),1000);
          HAL_Delay(1000);
        }
        break;
      case 3:
        HAL_Delay(100);
        r_yaw_ref = IMU_SurveyBias(GYROREFTIME);
        //Straight(FULL_SECTION,0,0,0);
        while(1){
          Straight(FULL_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);
          SpinTurn(90,TURN_ACCEL,TURN_SPEED,LEFT);
          Straight(FULL_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);
          SpinTurn(90,TURN_ACCEL,TURN_SPEED,LEFT);
          Straight(FULL_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);
          SpinTurn(90,TURN_ACCEL,TURN_SPEED,LEFT);
          Straight(FULL_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);
          SpinTurn(90,TURN_ACCEL,TURN_SPEED,RIGHT);
          Straight(FULL_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);
          SpinTurn(90,TURN_ACCEL,TURN_SPEED,RIGHT);
          Straight(FULL_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);
          SpinTurn(90,TURN_ACCEL,TURN_SPEED,RIGHT);
          Straight(FULL_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);
          SpinTurn(90,TURN_ACCEL,TURN_SPEED,RIGHT);
          Straight(FULL_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);
          SpinTurn(90,TURN_ACCEL,TURN_SPEED,LEFT);
        }
        //Straight(FULL_SECTION,0,0,0);
        /*
        while(1){
          ITM_SendChar((uint8_t)(spd*1000),1);
          HAL_Delay(10);
        }
        */
        break;
      case 4:
        r_yaw_ref = IMU_SurveyBias(GYROREFTIME);
        cnt=0;
        HAL_TIM_Base_Start_IT(&htim11);  //interrupt 1kHz

        tvL=1.0;
        tvR=1.0;
        runmode=TEST_MODE;
        HAL_Delay(3000);

        //Straight(FULL_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);
        runmode = DISABLE_MODE;

        HAL_TIM_Base_Stop_IT(&htim11);

        HAL_FLASH_Unlock();

        for(int i=0;i<cnt;i++){
          for(int j=0;j<4;j++){
            uint32_t _adrs = 0x080E0000 + (i*4+j)*sizeof(uint32_t);
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,_adrs,save[j][i]);
          }
        }
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,0x080FFFF0,cnt);

        HAL_FLASH_Lock();
        break;
      case 5:
        tvL = tvR = 0;
        runmode = TEST_MODE;
        for(float v=0;v<3.0;v+=0.1){
          tvL=tvR=v;
          HAL_Delay(100);
        }
        HAL_Delay(3000);
        runmode = DISABLE_MODE;
        HAL_Delay(500);
        break;
      case 6:
        led = 0b000;
        if(FlashMemory() != 0){
          printf("Sector Initialize Failed.\r\n");
          break;
        }
        Blink(10);
        break;
      case 7:
        {
          uint32_t _adrs = 0x080E0000;
          cnt = *(uint32_t*)(0x080FFFF0);
          printf("%ld\r\n",cnt);

          int wait = 0;

          while(_adrs<0x080E0000+sizeof(int32_t)*4*cnt){
            printf("%ld,",*(int32_t*)_adrs);
            _adrs+=sizeof(int32_t);
            printf("%ld,",*(int32_t*)_adrs);
            _adrs+=sizeof(int32_t);
            printf("%ld,",*(int32_t*)_adrs);
            _adrs+=sizeof(int32_t);
            printf("%ld\r\n",*(int32_t*)_adrs);
            _adrs+=sizeof(int32_t);
            if(wait>=1000){
              wait = 0;
              HAL_Delay(15000);
              printf("\r\n\r\n\033[2J");
            }else{
              wait++;
            }
          } 
          Blink(10);
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
  spd = 0.;
  led = mode;

  runmode = DISABLE_MODE;
}