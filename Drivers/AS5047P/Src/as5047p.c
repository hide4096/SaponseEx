#include"as5047p.h"

static SPI_HandleTypeDef* henc;
static GPIO_TypeDef* _cs_port;
static uint16_t _cs_pin;

static uint8_t CalcEvenParity(uint16_t data){
    uint8_t parity = 0;
    while(data){
        parity ^= data & 1;
        data >>= 1;
    }
    return parity;
}

static inline uint16_t GenerateSendCommand(uint16_t data,uint8_t rw){
    uint16_t command = 0;
    command = (data & 0x3FFF) + (rw << 14);
    command = command + (CalcEvenParity(command) << 15);
    return command;
}


static int readRegister(uint16_t address,uint16_t* data){
    uint16_t send = GenerateSendCommand(address,1);
    uint8_t _send[2];
    _send[0] = send >> 8;
    _send[1] = send & 0xFF;
    uint8_t _recv[2];

    HAL_GPIO_WritePin(_cs_port,_cs_pin,GPIO_PIN_RESET);
    HAL_SPI_Transmit(henc,_send,2,HAL_MAX_DELAY);
    HAL_SPI_Receive(henc,_recv,2,HAL_MAX_DELAY);
    HAL_GPIO_WritePin(_cs_port,_cs_pin,GPIO_PIN_SET);

    uint16_t recv = _recv[0] << 8 | _recv[1];

    if(CalcEvenParity(recv) != recv>>15){
        printf("Parity error\r\n");
        return -1;
    }
    if(recv & 0x4000){
        printf("Error occurred\r\n");
        return -1;
    }

    *data = recv & 0x3FFF;
    return 0;
}

static int writeRegister(uint16_t address,uint16_t data){
    uint16_t send[2];
    send[0] = GenerateSendCommand(address,0);
    send[1] = GenerateSendCommand(data,0);
    uint8_t _send[4];
    _send[0] = send[0] >> 8;
    _send[1] = send[0] & 0xFF;
    _send[2] = send[1] >> 8;
    _send[3] = send[1] & 0xFF;

    HAL_GPIO_WritePin(_cs_port,_cs_pin,GPIO_PIN_RESET);
    HAL_SPI_Transmit(henc,_send,4,HAL_MAX_DELAY);
    HAL_GPIO_WritePin(_cs_port,_cs_pin,GPIO_PIN_SET);

    return 0;
}

//ここから外部公開関数

uint8_t WHOAMI(){
    uint16_t recv;
    readRegister(0x0001,&recv);
    return recv;
}