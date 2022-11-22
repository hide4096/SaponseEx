/* 
#############################
    Wall Sensor & voltage
#############################
*/

#include"analog.h"

uint16_t adcval[5];
uint16_t sensval[4];
float vbat = 0;

static uint8_t senstype = 0;

void TrigWallSens(){
  senstype = !senstype;
  if(senstype){
    HAL_GPIO_WritePin(LED_FR_L_GPIO_Port,LED_FR_L_Pin,1);
    HAL_GPIO_WritePin(LED_FL_R_GPIO_Port,LED_FL_R_Pin,0);
  }else{
    HAL_GPIO_WritePin(LED_FR_L_GPIO_Port,LED_FR_L_Pin,0);
    HAL_GPIO_WritePin(LED_FL_R_GPIO_Port,LED_FL_R_Pin,1);
  }
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcval, 5);
}

void FetchWallSens(){
  if(senstype){
    sensval[SL] = adcval[SL];
    sensval[FR] = adcval[FR];
  }else{
    sensval[SR] = adcval[SR];
    sensval[FL] = adcval[FL];
  }
  HAL_GPIO_WritePin(LED_FR_L_GPIO_Port,LED_FR_L_Pin,0);
  HAL_GPIO_WritePin(LED_FL_R_GPIO_Port,LED_FL_R_Pin,0);
}

void GetBattVoltage(){
  vbat = 3.3 * (adcval[4] / 4096.0) * 2.;
  vbat += VBATREF;
}