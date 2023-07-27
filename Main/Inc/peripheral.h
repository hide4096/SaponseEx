#ifndef PERIPHERAL_H
#define PERIPHERAL_H

#include"icm20648.h"
#include"as5047p.h"
#include"drv8212.h"

#define SAVE_SECTOR 0x080C0000
#define SAVE_SIZE 0x20000

struct wall_threshold{
    uint16_t ll;
    uint16_t l;
    uint16_t r;
    uint16_t rr;
};

struct HMmode{
    float a;
    float max_v;
    float ang_a;
    float max_w;
    float len;
    float deg;
    uint8_t mode;
    int8_t turndir;
};

extern struct opposedMotors motors;
extern struct as5047p encR;
extern struct as5047p encL;
extern struct drv8212 drvR;
extern struct drv8212 drvL;
extern const struct gain straight;
extern const struct gain turn;
extern const struct ffgain ff;
extern const struct mouse_feature sapoex;
extern struct mouse_physical mouse;
extern struct mouse_physical target;
extern struct HMmode target_HM;
extern const struct wall_threshold detect_wall;

void SetLED(uint8_t);
int FlashMemory();
void WriteMemory(void*,uint32_t);

#endif