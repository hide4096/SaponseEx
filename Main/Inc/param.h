#ifndef PARAMETER_H
#define PARAMETER_H

//モータ
#define MTPERIOD    1000    //モータのCounterPeriod

//センサ全般
#define MAXINITERR  10      //センサ初期化試行回数

//電圧
#define VBATREF 0.03
#define LOWVOLT 3.3

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
#define SEARCH_ACCEL    1.0     //探索走行の加速度[m/s^2]
#define SEARCH_SPEED    0.2     //探索走行の速度[m/s]
#define MIN_ANGVEL      (M_PI/2.)     //旋回の最低速度[rad/s]
#define TURN_ACCEL      (M_PI)     //旋回加速度[rad/s^2]
#define TURN_SPEED      (M_PI*4.)    //旋回の最高速度[rad/s]
#define DELAY 100
#define ADCWAIT 10

//迷路情報
#define GOAL_X 9
#define GOAL_Y 9
#define MAZESIZE_X 16
#define MAZESIZE_Y 16
#define FULL_SECTION 90
#define HALF_SECTION 45

//閾値
#define WALL_TH_R   11
#define WALL_TH_L   7
#define WALL_TH_FR  65
#define WALL_TH_FL  45
#define REF_FR  417
#define REF_FL  268 
#define CONFIRM 1500

//PIDゲイン
#define SPD_KP 10.0
#define SPD_KI 5.0
#define SPD_KD 0.0
#define ANGVEL_KP 1.0
#define ANGVEL_KI 5.0
#define ANGVEL_KD 0.0
#define WALL_KP 0.0
#define WALL_KI 0.001

//Iゲイン上限
#define SPD_I_MAX       10000
#define ANGVEL_I_MAX    10000
#define WALLERROR_I_MAX 100000

//可読性向上
#define DISABLE_MODE 0
#define ENABLE_MODE 1
#define STRAIGHT_MODE 1
#define TURN_MODE 2
#define TEST_MODE 4
#define LEFT    -1
#define RIGHT   1
#define SL 0
#define FL 1
#define FR 2
#define SSR 3
#define NOWALL  0
#define ISWALL  1
#define UNKNOWN 2
#define SEARCH_MASK 0b01

#endif