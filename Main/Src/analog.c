#include"analog.h"

const uint16_t FLASH_WAIT = 30;
const float RR_GAIN = 0.75f;

struct analog sensor={
  .ll = 0,
  .l = 0,
  .r = 0,
  .rr = 0,
  .vbat = 0
};

static inline void _SETIR(uint8_t FRL,uint8_t FLR){
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

static inline int16_t CutNegative(int16_t number){
  return number*(number>0);
}

float GetBattVoltage(){
  return ((float)GetADCValue(9)/4095) * 4.9052;
}

void TrigWallSens(){
    static uint8_t flip = 0;
    if(flip){
        sensor.vbat = GetBattVoltage();

        uint16_t darkA = GetADCValue(1);
        uint16_t darkB = GetADCValue(3);
        _SETIR(1,0);
        for(int i=0;i<FLASH_WAIT;i++);
        sensor.ll = CutNegative(GetADCValue(1) - darkA);
        sensor.r = CutNegative(GetADCValue(3) - darkB);
    }else{
        uint16_t darkA = GetADCValue(2);
        uint16_t darkB = GetADCValue(4);
        _SETIR(0,1);
        for(int i=0;i<FLASH_WAIT;i++);
        sensor.l = CutNegative(GetADCValue(2) - darkA);
        sensor.rr = CutNegative(GetADCValue(4) - darkB)*RR_GAIN;
    }
    _SETIR(0,0);
    flip = !flip;
}