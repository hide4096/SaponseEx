/* 
########################
    Show Information
########################
*/

#include"show.h"

uint8_t led = 0;

void Blink(uint16_t val){
  led = 0b111;
  for(uint16_t i = 0; i < val;i++){
    led = ~led;
    HAL_Delay(50);
  }
}

void SetLED(){
  HAL_GPIO_WritePin(D3_GPIO_Port,D3_Pin,!(led & 0b001));
  HAL_GPIO_WritePin(D4_GPIO_Port,D4_Pin,!(led & 0b010));
  HAL_GPIO_WritePin(D5_GPIO_Port,D5_Pin,!(led & 0b100));
}