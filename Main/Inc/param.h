#ifndef PARAMETER_H
#define PARAMETER_H

//モータ
#define MTPERIOD    1000    //モータのCounterPeriod

//センサ全般
#define MAXINITERR  10      //センサ初期化試行回数

//電圧
#define LOWVOLT (3.3+0.18)

//ジャイロ
#define GYROREFTIME 1024
#define K 1.00
#define IMULPF 0.1

//エンコーダ
#define ENC_MAX     16384   //エンコーダ1回転のステップ数
#define ENC_HALF    8192
#define ALPHA       0.40

//機体情報
#define TIRE_DIAM   13.4   //タイヤ直径[mm]
#define PPMM        (TIRE_DIAM*M_PI)/ENC_MAX
#define DELTA_T     0.0005
#define MIN_SPEED   0.1
#define SEARCH_ACCEL    4.0     //探索走行の加速度[m/s^2]
#define SEARCH_SPEED    0.4     //探索走行の速度[m/s]
#define MIN_ANGVEL      (M_PI)     //旋回の最低速度[rad/s]
#define TURN_ACCEL      (M_PI*10.)     //旋回加速度[rad/s^2]
#define TURN_SPEED      (M_PI*7.)    //旋回の最高速度[rad/s]
#define SLALOM_ACCEL      (M_PI*40.)     //旋回加速度[rad/s^2]
#define SLALOM_SPEED      (M_PI*10.)    //旋回の最高速度[rad/s]
#define EXTEND 12
#define DELAY 100
#define ADCWAIT 10
#define WHEELDIST 35.7  //トレッド幅[mm]
#define LOOPFREQ 1000
#define FLASH_WAIT 30

//迷路情報
#define GOAL_X 3
#define GOAL_Y 3 
#define MAZESIZE_X 19
#define MAZESIZE_Y 19
#define FULL_SECTION 90
#define HALF_SECTION 45

//閾値
#define WALL_TH_R   10
#define WALL_TH_L   10
#define WALL_TH_FR  80
#define WALL_TH_FL  47
#define REF_FR  423
#define REF_FL  361
#define CONFIRM 2000

//PIDゲイン
#define R_DIFF 0.3
#define SPD_KP 20.
#define SPD_KI 160.
#define SPD_KD 0.001
#define ANGVEL_KP 0.6
#define ANGVEL_KI 20.0
#define ANGVEL_KD 0.01
#define WALL_KP 1.5
#define WALL_KI 0.
#define KX  0.1
#define KY  0.1
#define KT  0.1

//Iゲイン上限
#define SPD_I_MAX       10000
#define ANGVEL_I_MAX    10000
#define WALLERROR_I_MAX 100000

//可読性向上
#define DISABLE_MODE 0
#define ENABLE_MODE 1
#define STRAIGHT_MODE 1
#define TURN_MODE 2
#define CHASE_MODE 3
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