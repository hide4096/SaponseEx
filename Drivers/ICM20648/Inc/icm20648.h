/* 
########################
    ICM-20648 Driver
########################
    Date			Author              Notes
    2022/10/18      Aso Hidetoshi       First release
*/
#ifndef ICM20648_H
#define ICM20648_H

#include"spi.h"
#include"main.h"

int IMU_init(SPI_HandleTypeDef*,GPIO_TypeDef*,uint16_t);
float IMU_SurveyBias(int);

uint8_t  readWHO_AM_I();

int16_t accelX_raw();
int16_t accelY_raw();
int16_t accelZ_raw();
int16_t gyroX_raw();
int16_t gyroY_raw();
int16_t gyroZ_raw();

float accelX();
float accelY();
float accelZ();
float gyroX();
float gyroY();
float gyroZ();

#endif