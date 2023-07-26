#include"peripheral.h"

struct drv8212 drvR = {
    .hin1 = &htim1,
    .hin2 = &htim2,
    .in1_ch = TIM_CHANNEL_1,
    .in2_ch = TIM_CHANNEL_3,
    .is_reverse = TRUE,
};

struct drv8212 drvL = {
    .hin1 = &htim1,
    .hin2 = &htim1,
    .in1_ch = TIM_CHANNEL_3,
    .in2_ch = TIM_CHANNEL_2,
    .is_reverse = FALSE,
};

struct opposedMotors motors = {
    .hdrvR = &drvR,
    .hdrvL = &drvL,
};

struct as5047p encR = {
    .henc = &hspi1,
    .cs_port = GPIOB,
    .cs_pin = GPIO_PIN_2,
    .is_reverse = FALSE
};

struct as5047p encL = {
    .henc = &hspi1,
    .cs_port = GPIOA,
    .cs_pin = GPIO_PIN_0,
    .is_reverse = TRUE
};

struct mouse_physical target = {
    .v = 0,
    .w = 0
};

struct mouse_physical mouse = {
    .v = 0,
    .w = 0
};

const struct gain straight = {
    .Kp = 0.0f,
    .Ki = 0.0f,
    .Kd = 0.0f
};

const struct gain turn = {
    .Kp = 10.0f,
    .Ki = 0.2f,
    .Kd = 0.1f
};

const struct ffgain ff = {
    .ka = 250.0f,
    .kv = 1.0f
};

const struct mouse_feature sapoex = {
    .diam = 12.5f,
    .m = 27.4f,
    .gear_ratio = 4.2222,
    .ke = 0.062f,
    .r = 20.0f,
    .kT = 0.594f
};

void SetLED(uint8_t state){
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,!(state&0b001));
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_14,!(state&0b010));
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_15,!(state&0b100));
}

int FlashMemory(){
    FLASH_EraseInitTypeDef erase;

    erase.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase.Sector = FLASH_SECTOR_10;
    erase.NbSectors = 2;
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    uint32_t errcode = 0;

    HAL_FLASH_Unlock();
    HAL_FLASHEx_Erase(&erase,&errcode);
    HAL_FLASH_Lock();

    if(errcode == 0xFFFFFFFF) return 0;
    else return -1;
}

void WriteMemory(void *write,uint32_t size){
    uint32_t adrs = SAVE_SECTOR,write_size = 0;
    HAL_FLASH_Unlock();
    while(adrs < SAVE_SECTOR + SAVE_SIZE){
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,adrs,*(uint8_t*)(write+write_size));
        adrs += 1;
        write_size++;
        if(size <= write_size) break;
    }
    HAL_FLASH_Lock();
}
