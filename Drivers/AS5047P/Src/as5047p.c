#include"as5047p.h"

static SPI_HandleTypeDef* henc;
static GPIO_TypeDef* _cs_port;
static uint16_t _cs_pin;

static uint8_t CalcEvenParity(uint16_t data){
    uint8_t parity = 0;
    data &= 0x7FFF;
    while(data){
        parity ^= data & 1;
        data >>= 1;
    }
  return parity;
}

static inline uint16_t GenerateSendCommand(uint16_t data,uint8_t rw){
    uint16_t command;
    command = data & 0x3FFF;
    command |= rw << 14;
    command |= CalcEvenParity(command) << 15;
    return command;
}

int16_t readRegister(uint16_t address){
    uint16_t send = GenerateSendCommand(address,1);
    uint16_t recv = 0;

    HAL_GPIO_WritePin(_cs_port,_cs_pin,GPIO_PIN_RESET);
    HAL_SPI_Transmit(henc,(uint8_t*)&send,1,HAL_MAX_DELAY);
    HAL_GPIO_WritePin(_cs_port,_cs_pin,GPIO_PIN_SET);

    HAL_GPIO_WritePin(_cs_port,_cs_pin,GPIO_PIN_RESET);
    HAL_SPI_Receive(henc,(uint8_t*)&recv,1,HAL_MAX_DELAY);
    HAL_GPIO_WritePin(_cs_port,_cs_pin,GPIO_PIN_SET);

    if(CalcEvenParity(recv) != recv>>15){
        //printf("Parity error\r\n");
        return -1;
    }
    if(recv & 0x4000){
        //printf("Error occurred\r\n");
        return -1;
    }


    return recv & 0x3FFF;
}

//ここから外部公開関数
int as5047p_init(SPI_HandleTypeDef* handle,GPIO_TypeDef* port,uint16_t pin){
    henc = handle;
    _cs_port = port;
    _cs_pin = pin;

    HAL_GPIO_WritePin(_cs_port,_cs_pin,GPIO_PIN_SET);
    readRegister(0x0001);
    if(readRegister(0x0001) != 0){
        return -1;
    }
    return 0;
}

int16_t readAngle(void){
  return readRegister(0x3FFF);
}