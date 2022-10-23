/* 
###############################
    System Interrupt & Init
###############################
*/
#include"system.h"

extern TIM_HandleTypeDef htim6;
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern ADC_HandleTypeDef hadc1;
extern AS5047P_Instance encR;
extern AS5047P_Instance encL;

extern uint8_t motpower;
extern float r_yaw_ref;

void DoPanic(){
  SetDutyRatio(0,0);
  motpower = 0;
  HAL_TIM_Base_Stop_IT(&htim6);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,0);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,0);
  printf("Soiya!!\r\n");
  while(1) Blink(100);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
  if(htim == &htim6){
    //1kHz
    TrigWallSens();
    GetSpeed();
    GetYawDeg();
    GetBattVoltage();

    ControlDuty();
    FailSafe();

    if(vbat < LOWVOLT) while(1) DoPanic();
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
  r_yaw_ref = 0;
  float r_yaw_ref_tmp = 0;
  for(uint16_t i = 0;i<GYROREFTIME;i++){
    r_yaw_ref_tmp += gyroZ();
    HAL_Delay(1);
  }
  r_yaw_ref = (float)(r_yaw_ref_tmp / GYROREFTIME);

  //Motor
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
  SetDutyRatio(0,0);
  HAL_GPIO_WritePin(FAN_GPIO_Port,FAN_Pin,0);
  motpower = 0;

  //LED
  SetLED(0b000);

  //Interrupt 1kHz
  HAL_TIM_Base_Start_IT(&htim6);
}