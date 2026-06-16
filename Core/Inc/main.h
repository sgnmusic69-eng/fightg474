/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

	
/*HDL*/
#include "my_spi.h"
/*FML*/
#include "ad9833.h"
#include "fft_fml.h"
#include "adc_fml.h"
/*BLL*/
#include "fft_bll.h"
#include "adc_bll.h"
#include "dds_bll.h"
/*APL*/
#include "fft_apl.h"
#include "adc_apl.h"
#include "dds_apl.h"

#include "string.h"
#include "tim.h"
#include "adc.h"
#include "arm_const_structs.h"
	
	
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

#define FFT_LENGTH 2048

#define ADC1_DMA_BUFFER_LENGTH FFT_LENGTH

/* TIM1 sample trigger: 100 MHz / (1 + 1) / (47 + 1) */
#define ADC1_SAMPLE_FREQ_HZ 1041666.667f

#define FFT_SEARCH_FREQ_MIN_HZ 20000U
#define FFT_SEARCH_FREQ_MAX_HZ 120000U
#define FFT_PRINT_STEP_HZ 5000U

/*APL*/



/*FFT_APL*/

extern uint8_t fft_proc_flag;


/*fft_fml*/
extern float fft_input_buffer[FFT_LENGTH * 2];//复数输入缓冲区（实部/虚部交织存储）
extern float fft_magnitude[FFT_LENGTH];       //幅度谱（功率值）
extern float window_buffer[FFT_LENGTH];       //汉宁窗系数
extern float fft_out[FFT_LENGTH / 2];         //最终输出幅度谱

extern float adc1_data[ADC1_DMA_BUFFER_LENGTH];
extern uint8_t adc1_deal_flag;
extern uint32_t adc1_dma_buffer[ADC1_DMA_BUFFER_LENGTH]; 
extern uint8_t adc1_proc_flag;

/*adc_bll*/





/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LCD_CS_Pin GPIO_PIN_6
#define LCD_CS_GPIO_Port GPIOC
#define LCD_DC_Pin GPIO_PIN_7
#define LCD_DC_GPIO_Port GPIOC
#define LCD_RST_Pin GPIO_PIN_8
#define LCD_RST_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
