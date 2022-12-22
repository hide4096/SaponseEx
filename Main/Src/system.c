/* 
###############################
    System Interrupt & Init
###############################
*/
#include"system.h"

static uint8_t mode = 4;
static uint8_t txbuf[64];

unsigned int timer = 0;
volatile uint32_t writeadrs = 0;
volatile uint32_t cnt = 0;

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
    FailSafe();
    SetLED();
    timer++;
  }
  else if(htim == &htim10){
    //4kHz
    TrigWallSens();
  }
  else if(htim == &htim11){
    if(writeadrs >= 0x080E0000 && writeadrs <= 0x080FFFFF){
      HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,writeadrs,(int32_t)(spd*1000.));
      writeadrs+=sizeof(int32_t);
      HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,writeadrs,(int32_t)(angvel*1000.));
      writeadrs+=sizeof(int32_t);
      cnt++;
    }
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
  led = 0b000;


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
        while(1){
          sprintf((char*)txbuf,"%d\t%d\t%d\t%d\r\n",sensval[SL],sensval[FL],sensval[FR],sensval[SSR]);
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

        HAL_FLASH_Unlock();
        writeadrs = 0x080E0000;
        cnt=0;
        HAL_TIM_Base_Start_IT(&htim11);  //interrupt 100Hz

        HAL_Delay(500);

        tvL = -2.5,tvR = 2.5;
        runmode = TEST_MODE;
        HAL_Delay(3000);
        runmode = DISABLE_MODE;

        HAL_Delay(500);

        HAL_TIM_Base_Stop_IT(&htim11);
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(uint32_t)(0x080FFFF0),cnt);
        HAL_FLASH_Lock();
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

          while(_adrs<0x080E0000+sizeof(int32_t)*2*cnt){
            printf("%ld,",*(uint32_t*)_adrs);
            _adrs+=sizeof(int32_t);
            led=(_adrs&0b111000)>>3;
            printf("%ld\r\n",*(uint32_t*)_adrs);
            _adrs+=sizeof(int32_t);
            led=(_adrs&0b111000)>>3;
          } 
          Blink(50);
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