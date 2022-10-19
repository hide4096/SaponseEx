#ifndef PARAMETER_H
#define PARAMETER_H

#define MAXINITERR  10      //センサ初期化試行回数
#define MTPERIOD    2000    //モータのCounterPeriod

#define ENC_MAX     16384   //エンコーダ1回転のステップ数
#define ENC_HALF    8192

#define TIRE_DIAM   12.5
#define PPMM        (TIRE_DIAM*M_PI)/ENC_MAX

#define GYROREFTIME 10000
#define VBATREF 0.02
#define IMULPF 0.01
#define ENCLPF 0.1

#define SPD_KP 6.0
#define SPD_KI 0.1
#define SPD_KD 0.0
#define DEG_KP 0.01
#define DEG_KI 0.001
#define DEG_KD 0.0

#define SPD_I_MAX 10000
#define DEG_I_MAX 10000

#define CONFIRM 3000
#endif