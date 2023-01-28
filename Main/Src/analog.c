/* 
#############################
    Wall Sensor & voltage
#############################
*/

#include"analog.h"

uint16_t adcval[9];
int16_t sensval[4];
float vbat = 0;

volatile int16_t offval[4];
static int adcrank = -1;

static void _SETIR(uint8_t FRL,uint8_t FLR){
  HAL_GPIO_WritePin(LED_FR_L_GPIO_Port,LED_FR_L_Pin,FRL);
  HAL_GPIO_WritePin(LED_FL_R_GPIO_Port,LED_FL_R_Pin,FLR);
}

void TrigWallSens(){
  adcrank=(adcrank+1)%9;
  switch(adcrank){
    case 0:
      _SETIR(0,0);
      break;
    case 2:
    case 4:
      _SETIR(1,0);
      for(int i=0;i<FLASH_WAIT;i++);
      break;
    case 6:
    case 8:
      _SETIR(0,1);
      for(int i=0;i<FLASH_WAIT;i++);
      break;
    default:
      _SETIR(0,0);
      for(int i=0;i<DARK_WAIT;i++);
      break;
  }

  hadc1.Instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;

  switch(adcrank){
    case 1:
    case 3:
    case 5:
    case 7:
      TrigWallSens();
      break;
  }
}

void FetchWallSens(){
  if(adcrank == 8){
    sensval[0] = (adcval[2] - adcval[1])*(adcval[2] > adcval[1]);
    sensval[1] = (adcval[6] - adcval[5])*(adcval[6] > adcval[5]);
    sensval[2] = (adcval[4] - adcval[3])*(adcval[4] > adcval[3]);
    sensval[3] = (adcval[8] - adcval[7])*(adcval[8] > adcval[7]);
  }
}

void GetBattVoltage(){
  vbat = 3.3 * ((float)adcval[0] / 4096.0) * 2.;
  vbat += VBATREF;
}