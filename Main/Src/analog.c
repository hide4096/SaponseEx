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

void InitADCDMA(){
  //HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcval, 5);
}

void TrigWallSens(){
  if(senstype){
    HAL_GPIO_WritePin(LED_FR_L_GPIO_Port,LED_FR_L_Pin,1);
    HAL_GPIO_WritePin(LED_FL_R_GPIO_Port,LED_FL_R_Pin,0);
  }else{
    HAL_GPIO_WritePin(LED_FR_L_GPIO_Port,LED_FR_L_Pin,0);
    HAL_GPIO_WritePin(LED_FL_R_GPIO_Port,LED_FL_R_Pin,1);
  }
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcval, 5);
  senstype = !senstype;
}

void FetchWallSens(){
  if(senstype == 0){
    sensval[0] = adcval[0];
    sensval[2] = adcval[2];
  }else{
    sensval[1] = adcval[1];
    sensval[3] = adcval[3];
  }
  HAL_GPIO_WritePin(LED_FR_L_GPIO_Port,LED_FR_L_Pin,0);
  HAL_GPIO_WritePin(LED_FL_R_GPIO_Port,LED_FL_R_Pin,0);
}

void GetBattVoltage(){
  vbat = 3.3 * (adcval[4] / 4096.0) * 2.;
  vbat += VBATREF;
}