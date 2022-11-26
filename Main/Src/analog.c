/* 
#############################
    Wall Sensor & voltage
#############################
*/

#include"analog.h"

uint16_t adcval[5];
uint16_t sensval[4];
float vbat = 0;

static uint16_t darkval[4];
static uint8_t senstype = 0;

void TrigWallSens(){
  switch(senstype){
    case 0:
      HAL_GPIO_WritePin(LED_FR_L_GPIO_Port,LED_FR_L_Pin,0);
      HAL_GPIO_WritePin(LED_FL_R_GPIO_Port,LED_FL_R_Pin,0);
      HAL_Delay(ADCWAIT);
      break;
    case 1:
      HAL_GPIO_WritePin(LED_FR_L_GPIO_Port,LED_FR_L_Pin,1);
      HAL_GPIO_WritePin(LED_FL_R_GPIO_Port,LED_FL_R_Pin,0);
      break;
    case 2:
      senstype=0;
      HAL_GPIO_WritePin(LED_FR_L_GPIO_Port,LED_FR_L_Pin,0);
      HAL_GPIO_WritePin(LED_FL_R_GPIO_Port,LED_FL_R_Pin,1);
      break;
  }
  senstype++;
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcval, 5);
}

void FetchWallSens(){
  switch(senstype){
    case 0:
      for(int i=0;i<4;i++) darkval[i] = adcval[i];
      break;
    case 1:
      sensval[SL] = adcval[SL] - darkval[SL];
      sensval[FR] = adcval[FR] - darkval[FR];
      break;
    case 2:
      sensval[SR] = adcval[SR] - darkval[SR];
      sensval[FL] = adcval[FL] - darkval[FL];
      break;
  }
  HAL_GPIO_WritePin(LED_FR_L_GPIO_Port,LED_FR_L_Pin,0);
  HAL_GPIO_WritePin(LED_FL_R_GPIO_Port,LED_FL_R_Pin,0);
}

void GetBattVoltage(){
  vbat = 3.3 * (adcval[4] / 4096.0) * 2.;
  vbat += VBATREF;
}