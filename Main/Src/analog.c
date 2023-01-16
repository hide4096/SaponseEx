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
  if(cnt%2 == 0){
    sensval[0]-=adcval[1];
    sensval[1]+=adcval[2];
    sensval[2]-=adcval[3];
    sensval[3]+=adcval[4];
  }else{
    sensval[0]+=adcval[1];
    sensval[1]-=adcval[2];
    sensval[2]+=adcval[3];
    sensval[3]-=adcval[4];
  }
  cnt++;
  if(cnt >= 20){
    sensval[0]/=10;
    sensval[1]/=10;
    sensval[2]/=10;
    sensval[3]/=10;
    cnt=0;
  }
}


void GetBattVoltage(){
  vbat = 3.3 * ((float)adcval[0] / 4096.0) * 2.;
  vbat += VBATREF;
}