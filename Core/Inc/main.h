/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include<stdio.h>
#include<stdlib.h>
#define ARM_MATH_CM4
#include"arm_math.h"
#include"system.h"
#include"interrupt.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define D3_Pin GPIO_PIN_13
#define D3_GPIO_Port GPIOC
#define D4_Pin GPIO_PIN_14
#define D4_GPIO_Port GPIOC
#define D5_Pin GPIO_PIN_15
#define D5_GPIO_Port GPIOC
#define CS_L_Pin GPIO_PIN_0
#define CS_L_GPIO_Port GPIOA
#define SENS_L_Pin GPIO_PIN_1
#define SENS_L_GPIO_Port GPIOA
#define SENS_FL_Pin GPIO_PIN_2
#define SENS_FL_GPIO_Port GPIOA
#define SENS_FR_Pin GPIO_PIN_3
#define SENS_FR_GPIO_Port GPIOA
#define SENS_R_Pin GPIO_PIN_4
#define SENS_R_GPIO_Port GPIOA
#define CLK_ENC_Pin GPIO_PIN_5
#define CLK_ENC_GPIO_Port GPIOA
#define MISO_ENC_Pin GPIO_PIN_6
#define MISO_ENC_GPIO_Port GPIOA
#define MOSI_ENC_Pin GPIO_PIN_7
#define MOSI_ENC_GPIO_Port GPIOA
#define CS_IMU_Pin GPIO_PIN_0
#define CS_IMU_GPIO_Port GPIOB
#define VBATT_Pin GPIO_PIN_1
#define VBATT_GPIO_Port GPIOB
#define CS_R_Pin GPIO_PIN_2
#define CS_R_GPIO_Port GPIOB
#define MOT_R_IN1_Pin GPIO_PIN_10
#define MOT_R_IN1_GPIO_Port GPIOB
#define CLK_IMU_Pin GPIO_PIN_13
#define CLK_IMU_GPIO_Port GPIOB
#define MISO_IMU_Pin GPIO_PIN_14
#define MISO_IMU_GPIO_Port GPIOB
#define MOSI_IMU_Pin GPIO_PIN_15
#define MOSI_IMU_GPIO_Port GPIOB
#define MOT_R_IN2_Pin GPIO_PIN_8
#define MOT_R_IN2_GPIO_Port GPIOA
#define MOT_L_IN2_Pin GPIO_PIN_9
#define MOT_L_IN2_GPIO_Port GPIOA
#define MOT_L_IN1_Pin GPIO_PIN_10
#define MOT_L_IN1_GPIO_Port GPIOA
#define FAN_Pin GPIO_PIN_15
#define FAN_GPIO_Port GPIOA
#define LED_FR_L_Pin GPIO_PIN_4
#define LED_FR_L_GPIO_Port GPIOB
#define LED_FL_R_Pin GPIO_PIN_5
#define LED_FL_R_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
