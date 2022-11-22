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
#define TIRE_DIAM   13.23   //タイヤ直径[mm]
#define PPMM        (TIRE_DIAM*M_PI)/ENC_MAX
#define DELTA_T     0.0005
#define FAILSAFE    8.0   //単位はm/s
#define MIN_SPEED   0.1
#define SEARCH_ACCEL 0.8
#define SEARCH_SPEED 1.0
#define MIN_ANGVEL 90.
#define TURN_ACCEL 90.
#define TURN_SPEED 360.

//迷路情報
#define HALF_SECTION 45

//閾値
#define WALL_TH_R   50
#define WALL_TH_L   50
#define WALL_TH_FR  150
#define WALL_TH_FL  100
#define REF_FR  2000
#define REF_FL  1800 
#define REF_R   1200
#define REF_L   1200 
#define CONFIRM 2000

//PIDゲイン
#define SPD_KP 4.0
#define SPD_KI 0.03
#define SPD_KD 0.0
#define ANGVEL_KP 0.02
#define ANGVEL_KI 0.0
#define ANGVEL_KD 0.0
#define WALL_KP 0.1
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
#define LEFT    -1
#define RIGHT   1
#define SL 0
#define FL 1
#define FR 2
#define SR 3

#endif