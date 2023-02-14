/* 
###############################
    System Interrupt & Init
###############################
*/
#include"system.h"

static uint8_t mode = 1;
static uint8_t txbuf[64];

unsigned int timer = 0;
uint32_t cnt = 0;
int32_t save[4][4096];

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
    CalcPosition();
  }
  else if(htim == &htim7){
    //1kHz
    ControlDuty();
    FailSafe();
    SetLED();
    timer++;
  }
  else if(htim == &htim10){
    //2kHz
    TrigWallSens();
  }
  else if(htim == &htim11){
      save[0][cnt] = (int32_t)(spd*100000);
      save[1][cnt] = (int32_t)(len*100);
      save[2][cnt] = (int32_t)(deg*100);
      save[3][cnt] = (int32_t)(tgt_spd*100000);
      cnt++;
  }
}

void StartLogging(){
  cnt = 0;
  HAL_TIM_Base_Start_IT(&htim11);  //interrupt 1kHz
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

  //消灯
  HAL_GPIO_WritePin(LED_FR_L_GPIO_Port,LED_FR_L_Pin,0);
  HAL_GPIO_WritePin(LED_FL_R_GPIO_Port,LED_FL_R_Pin,0);

  HAL_TIM_Base_Start_IT(&htim6);  //interrupt 2kHz
  HAL_TIM_Base_Start_IT(&htim7);  //interrupt 1kHz
  HAL_TIM_Base_Start_IT(&htim10);  //interrupt 4kHz
}

void mainmenu(){
  I_angvel = 0;
  angvel = 0;

  if(sensval[FL] + sensval[FR] >= CONFIRM*2){
    Blink(2);
    switch (mode){
      case 1: //探索走行
        Blink(2);

        led = 0b111;
        r_yaw_ref = IMU_SurveyBias(GYROREFTIME);
        deg = 0;
        timer = 0;
        x_mypos = 0;
        y_mypos = 0;
        dire_mypos = north;
        cnt = 0;

        led = 0b000;

        SearchAdachi(GOAL_X,GOAL_Y);


        if(FlashMemory() != 0){
          printf("Sector Initialize Failed.\r\n");
          break;
        }
        Blink(10);

        HAL_FLASH_Unlock();

        //迷路情報の書き込み
        /*
        for(int x=0;x<MAZESIZE_X;x++){
          for(int y=0;y<MAZESIZE_Y;y++){
            wall_azim w = wall[x][y];
            uint32_t _adrs = 0x080E0000 + (x*MAZESIZE_Y+y)*sizeof(uint32_t);
            uint32_t _burn = w.north*16777216 + w.south*65536 + w.west*256 + w.east;
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,_adrs,_burn);
          }
        }
        */

       //センサ値の書き込み
        uint32_t _adrs = 0x080E0000;
        for(int i =0;i<cnt;i++){
          for(int j=0;j<4;j++){
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,_adrs,save[j][i]);
            _adrs+=sizeof(int32_t);
          }
        }
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,0x080FFFF0,cnt);

        HAL_Delay(1000);
        while(sensval[FL] + sensval[FR] >= CONFIRM*2);
        break;
      case 2: //センサ値表示
        r_yaw_ref = IMU_SurveyBias(GYROREFTIME);
        deg = 0;
        len=0;
        pos_x=pos_y=0.;
        while(1){
          sprintf((char*)txbuf,"%d\t%d\t%d\t%d\r\n",sensval[0],sensval[1],sensval[2],sensval[3]);
          printf("WallSens(SL,FL,FR,SR)\r\n");
          printf("%s",txbuf);

          printf("Voltage\t%.2fV\r\n",vbat);
          printf("Degree\t%.2f°\r\n",deg);
          printf("Length\t%.2fmm\r\n",len);
          printf("x\t%.2f\r\n",pos_x);
          printf("y\t%.2f\r\n",pos_y);

          printf("\033[2J");

          ITM_SendChar(sensval[0]/16,1);
          ITM_SendChar(sensval[1]/16,2);
          ITM_SendChar(sensval[2]/16,3);
          ITM_SendChar(sensval[3]/16,4);
          HAL_Delay(50);
        }
        break;
      case 3: //宴会芸
        HAL_Delay(100);
        r_yaw_ref = IMU_SurveyBias(GYROREFTIME);
        //Straight(200,SEARCH_ACCEL,SEARCH_SPEED,0);
        Straight(200,0,0,0);
        HAL_Delay(100);
        break;
      case 4: //走行テスト
        if(FlashMemory() != 0){
          printf("Sector Initialize Failed.\r\n");
          break;
        }

        Blink(10);

        r_yaw_ref = IMU_SurveyBias(GYROREFTIME);
        len = 0;
        deg = 0;

        StartLogging();

        Straight(200,SEARCH_ACCEL,SEARCH_SPEED,0);
        HAL_Delay(100);

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
      case 5: //電圧指定して走行
        tvL = tvR = 0;
        runmode = TEST_MODE;
        for(float v=0;v<3.0;v+=0.1){
          HAL_Delay(100);
        }
        HAL_Delay(3000);
        runmode = DISABLE_MODE;
        HAL_Delay(500);
        break;
      case 6:{  //ログダンプ
          uint32_t _adrs = 0x080E0000;
          for(int i=0;i<MAZESIZE_X*MAZESIZE_Y;i++){
            uint32_t _fetch = *(uint32_t*)_adrs;
            printf("%d,%d,%d,%d\r\n",(int)_fetch>>24,(int)(_fetch&0xFF0000)>>16,(int)(_fetch&0xFF00)>>8,(int)_fetch&0xFF);
            _adrs+=sizeof(uint32_t);
          }
          Blink(10);
        }
        break;
      case 7: //走行ログダンプ
        {
          uint32_t _adrs = 0x080E0000;
          const uint32_t ADRS_END = _adrs + sizeof(int32_t)*4*cnt;
          cnt = *(uint32_t*)(0x080FFFF0);
          printf("%ld\r\n",cnt);

          int wait = 0;

          while( _adrs < ADRS_END ){
            for(int i=0;i<2;i++){
              int32_t _recv = *(int32_t*)_adrs;
              printf("%d,",(int16_t)(_recv>>16));
              printf("%d,",(int16_t)_recv);
              _adrs+=sizeof(int32_t);
            }

            printf("%ld,",*(int32_t*)_adrs);
            _adrs+=sizeof(int32_t);
            printf("%ld\r\n",*(int32_t*)_adrs);
            _adrs+=sizeof(int32_t);

            //1000行おきに待機
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