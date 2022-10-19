/* 
########################
    Show Information
########################
*/

#include"show.h"

extern TIM_HandleTypeDef htim6;

void Blink(uint16_t val){
  uint8_t l_sig = 0b111;
  for(uint16_t i = 0; i< val;i++){
    l_sig = ~l_sig;
    SetLED(l_sig);
    HAL_Delay(50);
  }
}

void SetLED(uint8_t led){
  HAL_GPIO_WritePin(D3_GPIO_Port,D3_Pin,!(led & 0b001));
  HAL_GPIO_WritePin(D4_GPIO_Port,D4_Pin,!(led & 0b010));
  HAL_GPIO_WritePin(D5_GPIO_Port,D5_Pin,!(led & 0b100));
}

void DoPanic(){
  SetDutyRatio(0,0,0);
  motpower = 0;
  HAL_TIM_Base_Stop_IT(&htim6);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,0);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,0);
  printf("Soiya!!\r\n");
  while(1) Blink(100);
}