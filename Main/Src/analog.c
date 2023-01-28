/* 
#############################
    Wall Sensor & voltage
#############################
*/

#include"analog.h"

uint16_t adcval[5];
int16_t sensval[4];
int16_t senstmp[4];
float vbat = 0;

volatile int16_t offval[4];
static int flip = 0;
static int cnt = 0;

void FetchWallSens(){
  if(flip){
    senstmp[0]+=adcval[1];
    senstmp[2]+=adcval[3];
    senstmp[1]-=adcval[2];
    senstmp[3]-=adcval[4];
    ITM_SendChar(0,5);
  }else{
    senstmp[1]+=adcval[2];
    senstmp[3]+=adcval[4];
    senstmp[0]-=adcval[1];
    senstmp[2]-=adcval[3];
    ITM_SendChar(255,5);
  }
  flip=!flip;
  cnt++;
  if(cnt>=20){
    for(int i=0;i<4;i++){
      sensval[i] = senstmp[i] / 10.;
      senstmp[i] = 0;
    }
    cnt=0;
  }
}

void AverageSens(){
}


void GetBattVoltage(){
  vbat = 3.3 * ((float)adcval[0] / 4096.0) * 2.;
  vbat += VBATREF;
}