/* 
#############################
    Wall Sensor & voltage
#############################
*/

#include"analog.h"

int16_t sensval[4];
float vbat = 0;

static int flip = 1;

static void _SETIR(uint8_t FRL,uint8_t FLR){
  HAL_GPIO_WritePin(LED_FR_L_GPIO_Port,LED_FR_L_Pin,FRL);
  HAL_GPIO_WritePin(LED_FL_R_GPIO_Port,LED_FL_R_Pin,FLR);
}

static uint16_t GetADCValue(int channel){
  ADC_ChannelConfTypeDef sConfig = {0};
  sConfig.SamplingTime = ADC_SAMPLETIME_56CYCLES;
  sConfig.Rank = 1;
  switch(channel){
    case 1:
      sConfig.Channel = ADC_CHANNEL_1;
      break;
    case 2:
      sConfig.Channel = ADC_CHANNEL_2;
      break;
    case 3:
      sConfig.Channel = ADC_CHANNEL_3;
      break;
    case 4:
      sConfig.Channel = ADC_CHANNEL_4;
      break;
    default:
      sConfig.Channel = ADC_CHANNEL_9;
      break;
  }
  if(HAL_ADC_ConfigChannel(&hadc1,&sConfig) != HAL_OK){
    Error_Handler();
  }

  HAL_ADC_Start(&hadc1);
  HAL_ADC_PollForConversion(&hadc1,HAL_MAX_DELAY);

  uint16_t adcvalue = HAL_ADC_GetValue(&hadc1);

  HAL_ADC_Stop(&hadc1);

  return adcvalue;
}

static int16_t CutNegative(int16_t number){
  return number*(number>0);
}

void TrigWallSens(){
  if(flip){
    uint16_t darkA = GetADCValue(1);
    uint16_t darkB = GetADCValue(3);
    _SETIR(1,0);
    for(int i=0;i<FLASH_WAIT;i++);
    sensval[0] = CutNegative(GetADCValue(1) - darkA);
    sensval[2] = CutNegative(GetADCValue(3) - darkB);
    _SETIR(0,0);
  }else{
    uint16_t darkA = GetADCValue(2);
    uint16_t darkB = GetADCValue(4);
    _SETIR(0,1);
    for(int i=0;i<FLASH_WAIT;i++);
    sensval[1] = CutNegative(GetADCValue(2) - darkA);
    sensval[3] = CutNegative(GetADCValue(4) - darkB);
    _SETIR(0,0);

    GetBattVoltage();
  }

  flip=1-flip;
}

void GetBattVoltage(){
  vbat = ((float)GetADCValue(9)/4095) * 4.9052;
}