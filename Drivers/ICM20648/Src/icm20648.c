/* 
########################
    ICM-20648 Driver
########################
    Date			Author              Notes
    2022/10/18      Aso Hidetoshi       First release
*/

#include"icm20648.h"

#define WHO_AM_I 0xE0
#define RETRY_INIT 5
#define TIMEOUT_MS 100

static SPI_HandleTypeDef *himu;
static GPIO_TypeDef *_port_cs;
static uint16_t _pin_cs;
static float gyro_sensitivity=1.;
static float accel_sensitivity=1.;

static uint8_t IMU_readRegister(uint8_t _adrs){
    uint8_t recv = 0,adrs = _adrs | 0x80;

    HAL_GPIO_WritePin(_port_cs,_pin_cs,GPIO_PIN_RESET);
    HAL_SPI_Transmit(himu,&adrs,1,HAL_MAX_DELAY);
    HAL_SPI_Receive(himu,&recv,1,HAL_MAX_DELAY);
    HAL_GPIO_WritePin(_port_cs,_pin_cs,GPIO_PIN_SET);
    
    return recv; 
}

static uint16_t IMU_readRegister_2Byte(uint8_t _Hadrs,uint8_t _Ladrs){
    uint8_t Hrecv,Lrecv;
    Hrecv = IMU_readRegister(_Hadrs);
    Lrecv = IMU_readRegister(_Ladrs);
    return Hrecv << 8 | Lrecv;
}

static void IMU_writeRegister(uint8_t _adrs,uint8_t data){
    uint8_t adrs = _adrs & 0x7F;

    HAL_Delay(1);   //ちょっと待ってみる

    HAL_GPIO_WritePin(_port_cs,_pin_cs,GPIO_PIN_RESET);
    HAL_SPI_Transmit(himu,&adrs,1,HAL_MAX_DELAY);
    HAL_SPI_Transmit(himu,&data,1,HAL_MAX_DELAY);
    HAL_GPIO_WritePin(_port_cs,_pin_cs,GPIO_PIN_SET);
}

/*
    ジャイロ
    0   →   ±250dps
    1   →   ±500dps
    2   →   ±1000dps
    3   →   ±2000dps
    加速度
    0   →   ±2g
    1   →   ±4g
    2   →   ±8g
    3   →   ±16g
*/
static int IMU_changeSensitivity(uint8_t _gyro,uint8_t _accel){
    if(_gyro > 0b11 || _accel > 0b11) return -1;

    uint8_t gyro_config=_gyro << 1,accel_config=_accel<<1;

    IMU_writeRegister(0x7F,0x20);   //バンク切り替え(バンク2)
    
    //レジスタに書き込む
    IMU_writeRegister(0x01,gyro_config);
    IMU_writeRegister(0x14,accel_config);
    
    //書き込まれたか確認
    if((IMU_readRegister(0x01) & 0b0110) != gyro_config) return -1;
    if((IMU_readRegister(0x14) & 0b0110) != accel_config) return -1;

    IMU_writeRegister(0x7F,0x00);   //バンク切り替え(バンク0)

    switch(_gyro){
        case 0:
            gyro_sensitivity = 250.0 / 32767.;
            break;
        case 1:
            gyro_sensitivity = 500.0 / 32767.;
            break;
        case 2:
            gyro_sensitivity = 1000.0 / 32767.;
            break;
        case 3:
            gyro_sensitivity = 2000.0 / 32767.;
            break;
    }
    switch(_accel){
        case 0:
            accel_sensitivity = 2.0 / 32767.;
            break;
        case 1:
            accel_sensitivity = 4.0 / 32767.;
            break;
        case 2:
            accel_sensitivity = 8.0 / 32767.;
            break;
        case 3:
            accel_sensitivity = 16.0 / 32767.;
            break;
    }

    return 0;
}

uint8_t readWHO_AM_I(){
    return IMU_readRegister(0x00);
}

int IMU_init(SPI_HandleTypeDef *handle,GPIO_TypeDef *port,uint16_t pin){
    himu = handle;
    _port_cs = port;
    _pin_cs = pin;

    int errcnt = 0;
    uint8_t whoami = readWHO_AM_I();
    while(whoami != WHO_AM_I){
        HAL_Delay(100);
        whoami = readWHO_AM_I();
        printf("%x\r\n",whoami);
        errcnt++;
        if(errcnt >= RETRY_INIT) return -1;
    }

    //IMUリセット
    IMU_writeRegister(0x06,0b10000000);
    HAL_Delay(10);
    //スリープモード無効&温度センサ無効
    IMU_writeRegister(0x06,0b00001001);
    //レンジの調整
    IMU_changeSensitivity(3,1);

    return 0;
}

float IMU_SurveyBias(int _reftime){
    float r_yaw_ref_tmp = 0;
    for(uint16_t i = 0;i<_reftime;i++){
        r_yaw_ref_tmp += gyroZ();
        HAL_Delay(1);
    }
  return (float)(r_yaw_ref_tmp / GYROREFTIME);
}

int16_t accelX_raw(){
    return IMU_readRegister_2Byte(0x2D,0x2E);
}

int16_t accelY_raw(){
    return IMU_readRegister_2Byte(0x2F,0x30);
}

int16_t accelZ_raw(){
    return IMU_readRegister_2Byte(0x31,0x32);
}

int16_t gyroX_raw(){
    return IMU_readRegister_2Byte(0x33,0x34);
}

int16_t gyroY_raw(){
    return IMU_readRegister_2Byte(0x35,0x36);
}

int16_t gyroZ_raw(){
    return IMU_readRegister_2Byte(0x37,0x38);
}

float gyroX(){
    return (float)gyroX_raw() * gyro_sensitivity;
}

float gyroY(){
    return (float)gyroY_raw() * gyro_sensitivity;
}

float gyroZ(){
    return (float)gyroZ_raw() * gyro_sensitivity;
}

float accelX(){
    return (float)accelX_raw() * accel_sensitivity;
}

float accelY(){
    return (float)accelY_raw() * accel_sensitivity;
}

float accelZ(){
    return (float)accelZ_raw() * accel_sensitivity;
}