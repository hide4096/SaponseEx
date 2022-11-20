#ifndef PARAMETER_H
#define PARAMETER_H

//モータ
#define MTPERIOD    2000    //モータのCounterPeriod

//センサ全般
#define MAXINITERR  10      //センサ初期化試行回数

//電圧
#define VBATREF 0.11
#define LOWVOLT 3.2

//ジャイロ
#define GYROREFTIME 1000
#define IMULPF 0.1

//エンコーダ
#define ENC_MAX     16384   //エンコーダ1回転のステップ数
#define ENC_HALF    8192
#define ENCLPF 0.1

//機体情報
#define TIRE_DIAM   13.23   //タイヤ直径
#define PPMM        (TIRE_DIAM*M_PI)/ENC_MAX
#define DELTA_T     0.0005
#define FAILSAFE 8.0   //単位はm/s

//閾値
#define WALL_TH_R   50
#define WALL_TH_L   50
#define WALL_TH_FR  50
#define WALL_TH_FL  50
#define REF_FR  1200
#define REF_FL  1200 
#define REF_R   1200
#define REF_L   1200 
#define CONFIRM 2000

//PIDゲイン
#define SPD_KP 3.0
#define SPD_KI 0.05
#define SPD_KD 0.1
#define ANGVEL_KP 0.001
#define ANGVEL_KI 0.0001
#define ANGVEL_KD 0.001
#define WALL_KP 0.001
#define WALL_KI 0.0

//Iゲイン上限
#define SPD_I_MAX       10000
#define ANGVEL_I_MAX    10000
#define WALLERROR_I_MAX 100000

//可読性向上
#define DISABLE_MODE 0
#define ENABLE_MODE 1
#define STRAIGHT_MODE 1
#define TURN_MODE 2
#define SL 0
#define FL 1
#define FR 2
#define SR 3

#endif