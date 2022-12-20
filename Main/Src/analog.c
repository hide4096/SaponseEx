/* 
#############################
    Wall Sensor & voltage
#############################
*/

#include"analog.h"

uint16_t adcval[9];
int16_t sensval[4];
float vbat = 0;

int16_t darkval[4];
volatile uint8_t senstype = 0;


void TrigWallSens(){
  switch(senstype){
    case 0:
    case 1:
    case 3:
    case 5:
    case 7:
      HAL_GPIO_WritePin(LED_FL_R_GPIO_Port,LED_FL_R_Pin,0);
      HAL_GPIO_WritePin(LED_FR_L_GPIO_Port,LED_FR_L_Pin,0);
      break;
    case 2:
    case 6:
      HAL_GPIO_WritePin(LED_FL_R_GPIO_Port,LED_FL_R_Pin,1);
      HAL_GPIO_WritePin(LED_FR_L_GPIO_Port,LED_FR_L_Pin,0);
      break;
    case 4:
    case 8:
      HAL_GPIO_WritePin(LED_FL_R_GPIO_Port,LED_FL_R_Pin,0);
      HAL_GPIO_WritePin(LED_FR_L_GPIO_Port,LED_FR_L_Pin,1);
      break;
  }
  hadc1.Instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;
}

void FetchWallSens(){
  senstype=(senstype+1)%9;
  ITM_SendChar((uint8_t)(adcval[1]>>4),1);
  ITM_SendChar((uint8_t)(adcval[7]>>4),2);
  __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_EOC);
}

void GetBattVoltage(){
  vbat = 3.3 * ((float)adcval[0] / 4096.0) * 2.;
  vbat += VBATREF;
}