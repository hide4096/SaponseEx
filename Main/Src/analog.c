/* 
#############################
    Wall Sensor & voltage
#############################
*/

#include"analog.h"

uint16_t adcval[5];
int16_t sensval[4];
float vbat = 0;

volatile int16_t offval[4];
static int cnt = 0;

void FetchWallSens(){
  __NOP();
  cnt++;
  if(cnt >= 10)cnt=0;
}


void GetBattVoltage(){
  vbat = 3.3 * ((float)adcval[0] / 4096.0) * 2.;
  vbat += VBATREF;
}