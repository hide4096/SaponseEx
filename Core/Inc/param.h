#ifndef PARAMETER_H_
#define PARAMETER_H_

#define MAXINITERR  10      //センサ初期化試行回数
#define MTPERIOD    2000    //モータのCounterPeriod

#define ENC_MAX     16384   //エンコーダ1回転のステップ数
#define ENC_HALF    8192

#define TIRE_DIAM   12.5
#define PPMM        (TIRE_DIAM*M_PI)/ENC_MAX

#define GYROREFTIME 1000
#define VBATREF 0.02
#define IMULPF 0.01

#define CONFIRM 3000
#endif