#ifndef PARAMETER_H
#define PARAMETER_H

#define MAXINITERR  10      //センサ初期化試行回数
#define MTPERIOD    2000    //モータのCounterPeriod

#define ENC_MAX     16384   //エンコーダ1回転のステップ数
#define ENC_HALF    8192

#define TIRE_DIAM   13.23   //タイヤ直径
#define PPMM        (TIRE_DIAM*M_PI)/ENC_MAX
#define DELTA_T     0.0005

#define GYROREFTIME 1000
#define VBATREF 0.11
#define LOWVOLT 3.2

#define IMULPF 0.1
#define ENCLPF 0.1

#define SPD_KP 3.0
#define SPD_KI 0.05
#define SPD_KD 0.1
#define ANGVEL_KP 0.001
#define ANGVEL_KI 0.0001
#define ANGVEL_KD 0.001
#define WALL_KP 0.1

#define SPD_I_MAX 10000
#define ANGVEL_I_MAX 10000

#define FAILSAFE 10.0   //単位はm/s

#define CONFIRM 2000
#endif