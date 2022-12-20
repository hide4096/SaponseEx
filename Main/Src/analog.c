/* 
#############################
    Wall Sensor & voltage
#############################
*/

#include"analog.h"

uint16_t adcval[9];
int16_t sensval[4];
float vbat = 0;

volatile uint8_t sensseq = 0;
volatile int16_t offval[4];

void TrigWallSens(){
  hadc1.Instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;
}

void FetchWallSens(){
/*
  switch(sensseq){
    case 1:
    case 5:
      if(sensseq == 1)  offval[0] = adcval[sensseq];
      else              offval[2] = adcval[sensseq];
      HAL_GPIO_WritePin(LED_FL_R_GPIO_Port,LED_FL_R_Pin,1);
      for(int i=0;i<1000;i++);
      hadc1.Instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;
      break;
    case 3:
    case 7:
      if(sensseq == 3)  offval[1] = adcval[sensseq];
      else              offval[3] = adcval[sensseq];
      HAL_GPIO_WritePin(LED_FL_R_GPIO_Port,LED_FL_R_Pin,1);
      for(int i=0;i<1000;i++);
      hadc1.Instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;
      break;
    case 2:
    case 6:
      if(sensseq == 1)  sensval[0] = adcval[sensseq] - offval[0];
      else              sensval[2] = adcval[sensseq] - offval[2];
      HAL_GPIO_WritePin(LED_FL_R_GPIO_Port,LED_FL_R_Pin,0);
      break;
    case 4:
    case 8:
      if(sensseq == 4)  sensval[1] = adcval[sensseq] - offval[1];
      else              sensval[3] = adcval[sensseq] - offval[3];
      HAL_GPIO_WritePin(LED_FR_L_GPIO_Port,LED_FR_L_Pin,0);
      break;
  }
*/

  __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_EOC);
  //sensseq++;
}

void GetBattVoltage(){
  vbat = 3.3 * ((float)adcval[0] / 4096.0) * 2.;
  vbat += VBATREF;
}