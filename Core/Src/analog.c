/* 
#############################
    Wall Sensor & voltage
#############################
*/

#include"analog.h"

uint16_t adcval[5];
uint16_t sensval[4];

//0→FR,L 1→FL,R
static uint8_t senstype = 0;

float vbat = 0;

void GetWallSens(){
  if(senstype){
    sensval[0] = adcval[0];
    sensval[2] = adcval[2];
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,0);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,1);
  }else{
    sensval[1] = adcval[1];
    sensval[3] = adcval[3];
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,1);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,0);
  }
    senstype = 1-senstype;
}

void GetBattVoltage(){
  vbat = 3.3 * (adcval[4] / 4096.0) * 2;
  vbat += VBATREF;
}