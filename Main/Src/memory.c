#include"memory.h"

int FlashMemory(){
    FLASH_EraseInitTypeDef erase;

    erase.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase.Sector = FLASH_SECTOR_11;
    erase.NbSectors = 1;
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    uint32_t errcode = 0;

    HAL_FLASH_Unlock();
    HAL_FLASHEx_Erase(&erase,&errcode);
    HAL_FLASH_Lock();

    if(errcode == 0xFFFFFFFF) return 0;
    else return -1;
}