/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"param.h"
#include"icm20648.h"
#include"as5047p.h"
#define ARM_MATH_CM4
#include"arm_math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void SetLED(uint8_t led){
  HAL_GPIO_WritePin(D3_GPIO_Port,D3_Pin,!(led & 0b001));
  HAL_GPIO_WritePin(D4_GPIO_Port,D4_Pin,!(led & 0b010));
  HAL_GPIO_WritePin(D5_GPIO_Port,D5_Pin,!(led & 0b100));
}

uint8_t motpower = 0;
void   SetDutyRatio(int16_t motL,int16_t motR,int16_t motF){
  if(motpower){
    if(motR > 0){
      if(motR > MTPERIOD) motR = MTPERIOD;
     __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,motR);
     __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,0);
    }else{
      motR*=-1;
      if(motR > MTPERIOD) motR = MTPERIOD;
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,0);
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,motR);  
    }

    if(motL > 0){
      if(motL > MTPERIOD) motL = MTPERIOD;
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,motL);
      __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,0);
    }else{
      motL*=-1;
      if(motL > MTPERIOD) motL = MTPERIOD;
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,0);
      __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,motL);
    }

    if(motF > MTPERIOD) motF = MTPERIOD;
    __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,2000);
  }
}

//Enc
AS5047P_Instance encR;
AS5047P_Instance encL;
//spd
float spd = 0;
int16_t b_encR_val=0,b_encL_val=0;
float spdR = 0,spdL=0;
float b_spdR = 0,b_spdL=0;
void GetSpeed(){
  int16_t encR_val,encL_val;
  encR_val = AS5047P_ReadPosition(&encR, AS5047P_OPT_ENABLED);
  encL_val = AS5047P_ReadPosition(&encL, AS5047P_OPT_ENABLED);
	if(AS5047P_ErrorPending(&encR)) AS5047P_ErrorAck(&encR);
	if(AS5047P_ErrorPending(&encL)) AS5047P_ErrorAck(&encL);
  int16_t d_encR_val = encR_val - b_encR_val;
  int16_t d_encL_val = b_encL_val -  encL_val;
  if((d_encR_val > ENC_HALF || d_encR_val < -ENC_HALF) && b_encR_val > ENC_HALF){
    d_encR_val = ((ENC_MAX - 1) - b_encR_val) + encR_val;
  }
  else if((d_encR_val > ENC_HALF || d_encR_val < -ENC_HALF) && b_encR_val <= ENC_HALF){
    d_encR_val = (b_encR_val + ((ENC_MAX -1)-encR_val));
  }
  if((d_encL_val > ENC_HALF || d_encL_val < -ENC_HALF) && b_encL_val > ENC_HALF){
    d_encL_val = ((ENC_MAX - 1) - b_encL_val) + encL_val;
  }
  else if((d_encL_val > ENC_HALF || d_encL_val < -ENC_HALF) && b_encL_val <= ENC_HALF){
    d_encL_val = (b_encL_val + ((ENC_MAX -1)-encL_val));
  }

  float n_spdR = (float)d_encR_val * (float)PPMM;
  float n_spdL = (float)d_encL_val * (float)PPMM;
  b_spdR = spdR;
  b_spdL = spdL;

  spdR = n_spdR * ENCLPF + b_spdR * (1.0 - ENCLPF);
  spdL = n_spdL * ENCLPF + b_spdL * (1.0 - ENCLPF);

  spd = (spdR + spdL) / 2.0;  //
  b_encR_val = encR_val;
  b_encL_val = encL_val;
}

//0→FR,L 1→FL,R
uint8_t senstype = 0;
//DMAの送信(L,FL,FR,R,VBAT/2)
uint16_t adcval[5];
uint16_t sensval[4];
uint16_t offval[4];

void GetWallSens(){
  if(senstype){
    sensval[0] = adcval[0];
    sensval[2] = adcval[2];
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,0);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,1);
  }else{
    sensval[1] = adcval[1];
    sensval[3] = adcval[3];
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,1);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,0);
  }
    senstype = 1-senstype;
}

float vbat = 0;
void GetBattVoltage(){
  vbat = 3.3 * (adcval[4] / 4096.0) * 2;
  vbat += VBATREF;
}

float deg = 0;
float angvel = 0,b_angvel = 0;
float r_yaw = 0,r_yaw_new = 0,r_yaw_ref = 0,r_b_yaw;

void GetYawDeg(){
	r_yaw_new = gyroZ() - r_yaw_ref;
  r_b_yaw = r_yaw;
  r_yaw = r_yaw_new * IMULPF + r_b_yaw * (1.0 - IMULPF);
	b_angvel = angvel;
	angvel = (2000.0*r_yaw/32767.0)*PI/180.0;
	deg += 2.0*r_yaw/32767.0;
}

float tgt_spd = 0.0;
float tgt_deg = 0.0;
void ControlDuty(){
  float dutyR = 0.0,dutyL = 0.0;
  dutyR = (tgt_spd - spd)*6.0 + (tgt_deg - deg)*0.015;
  dutyL = (tgt_spd - spd)*6.0 - (tgt_deg - deg)*0.015;

  SetDutyRatio(MTPERIOD*dutyL,MTPERIOD*dutyR,0);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
  if(htim == &htim6){
    //1kHz
    GetWallSens();
    GetSpeed();
    GetYawDeg();
    GetBattVoltage();

    ControlDuty();
  }
}

void Blink(uint16_t val){
  uint8_t l_sig = 0b111;
  for(uint16_t i = 0; i< val;i++){
    l_sig = ~l_sig;
    SetLED(l_sig);
    HAL_Delay(50);
  }
}

void DoPanic(){
  SetDutyRatio(0,0,0);
  motpower = 0;
  HAL_TIM_Base_Stop_IT(&htim6);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,0);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,0);
  printf("Soiya!!\r\n");
  while(1) Blink(100);
}

void init(){
  HAL_Delay(500);
  
  //StartDMA
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcval, 5);

  //InitEnc
  uint8_t errcnt = 0;
  AS5047P_Init(&encL, 0);
  AS5047P_Init(&encR, 1);
  while(AS5047P_ErrorPending(&encR) || AS5047P_ErrorPending(&encL)){
    errcnt++;
    if(errcnt >= MAXINITERR) DoPanic();
    AS5047P_Init(&encL, 0);
    AS5047P_Init(&encR, 1);
    HAL_Delay(100);
  }
  AS5047P_SetZeroPosition(&encL);
  AS5047P_SetZeroPosition(&encR);
  errcnt = 0;

  //InitIMU
  if(IMU_init(&hspi2,CS_IMU_GPIO_Port,CS_IMU_Pin) < 0) DoPanic();

  r_yaw_ref = 0;
  float r_yaw_ref_tmp = 0;
  for(uint16_t i = 0;i<GYROREFTIME;i++){
    r_yaw_ref_tmp += gyroZ();
  }
  r_yaw_ref = (float)(r_yaw_ref_tmp / GYROREFTIME);

  //Motor
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
  SetDutyRatio(0,0,0);
  motpower = 0;

  //LED
  SetLED(0b000);
  

  //Interrupt 1kHz
  HAL_TIM_Base_Start_IT(&htim6);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI2_Init();
  MX_ADC1_Init();
  MX_SPI1_Init();
  MX_USART6_UART_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint8_t mode = 1;
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if(spd > 0.1){
      mode++;
      Blink(5);
    }else if(spd < -0.1){
      mode--;
      Blink(5);
    }else if(sensval[0] + sensval[3] >= CONFIRM*2){
      switch (mode){
        case 1:
          while(1){
            printf("%fm/s\t%fdeg/s\t%fdeg\r\n",spd,angvel,deg);
            HAL_Delay(100);
          }
          break;
        case 2:
          SetLED(0b000);
          HAL_Delay(500);
          motpower = 1;
          tgt_spd = 0.2;
          tgt_deg = deg;
          HAL_Delay(1000);
          tgt_spd = 0.0;
          HAL_Delay(1000);
          break;
        case 3:
          motpower = 1;
          tgt_deg = 0.0;
          tgt_spd = 0;
          while (1);
          break;
        case 4:
          while(1){
            printf("%f\r\n",accelX());
          }
          break;
        case 5:
          SetLED(0b000);
          float avr_vbat = 0.0;
          for(int i = 0;i<100;i++){
            avr_vbat += vbat;
            HAL_Delay(1);
          }
          avr_vbat /= 100;
          printf("%.3fv\r\n",avr_vbat);
          printf("%d\r\n",adcval[4]);
          break;
        case 6:
          motpower=1;
          double cnt = 0;
          while(1){
            SetDutyRatio(700*sin(cnt),700*sin(cnt),0);
            cnt+=PI/2000;
            HAL_Delay(1);
            if(cnt>2*PI) cnt = 0;
          }
          break;
        case 7:
          motpower = 1;
          while(1){
            SetDutyRatio(500,500,0);
          }
          break;
        default:
          DoPanic();
          break;
      }
    }
    SetLED(mode);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 10;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
int _write(int file, char *ptr, int len)
{
  int DataIdx;
  for(DataIdx=0; DataIdx<len; DataIdx++)
  {
    ITM_SendChar(*ptr++);
  }
  return len;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
