#include"as5047p.h"

#define RETRY_INIT 5

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

static int16_t readRegister(struct as5047p* enc,uint16_t address){
    uint16_t send = GenerateSendCommand(address,1);
    uint16_t recv = 0;

    HAL_GPIO_WritePin(enc->cs_port,enc->cs_pin,GPIO_PIN_RESET);
    HAL_SPI_Transmit(enc->henc,(uint8_t*)&send,1,HAL_MAX_DELAY);
    HAL_GPIO_WritePin(enc->cs_port,enc->cs_pin,GPIO_PIN_SET);

    HAL_GPIO_WritePin(enc->cs_port,enc->cs_pin,GPIO_PIN_RESET);
    HAL_SPI_Receive(enc->henc,(uint8_t*)&recv,1,HAL_MAX_DELAY);
    HAL_GPIO_WritePin(enc->cs_port,enc->cs_pin,GPIO_PIN_SET);
    

    if(CalcEvenParity(recv) != recv>>15){
        printf("Parity error\r\n");
        return -1;
    }
    if(recv & 0x4000){
        printf("Error occurred\r\n");
        return -1;
    }

    return recv & 0x3FFF;
}

//ここから外部公開関数
int as5047p_init(struct as5047p* enc){
    uint8_t retrycnt = 0;
    while (retrycnt < RETRY_INIT){
        if(readRegister(enc,0x0001) == 0){
            return 0;
        }

        retrycnt++;
        HAL_Delay(10);
    }
    return -1;
}

int16_t readAngle(struct as5047p* enc){
    int16_t angle = readRegister(enc,0x3FFF);
    if(angle == -1){
        return -1;
    }
    if(enc->is_reverse){
        return AS5047P_MAX_ANGLE - angle;
    }else{
        return angle;
    }
}