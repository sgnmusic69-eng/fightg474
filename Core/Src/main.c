/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "comp.h"
#include "dac.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "st7735s.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define COMP_DAC_INIT_CODE 1600U
#define COMP_SCAN_START_MV 1650U
#define COMP_SCAN_END_MV   2650U
#define COMP_SCAN_STEP_MV  100U
#define COMP_TARGET_OUT_HZ 5000U
#define COMP_SCAN_WINDOW_MS 100U
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//extern UART_HandleTypeDef huart1;

volatile uint16_t comp_dac_code = COMP_DAC_INIT_CODE;
volatile uint32_t tim3_update_count = 0U;
volatile uint32_t tim3_update_count_last = 0U;
volatile uint8_t pd12_div_state = 0U;
volatile uint32_t comp_input_freq_hz = 0U;
volatile uint32_t tim3_divider_edges = 5U;
volatile uint32_t comp_best_error_hz = 0U;
volatile float adc_avg_v = 0.0f;
volatile float adc_min_v = 0.0f;
volatile float adc_max_v = 0.0f;
volatile float signal_a_hz = 0.0f;
volatile float signal_b_hz = 0.0f;
volatile float signal_a_mag = 0.0f;
volatile float signal_b_mag = 0.0f;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void tim3_div10_output_init(void);
static void format_float_3(char *buffer, size_t buffer_size, float value);
static uint16_t dac_code_from_mv(uint32_t mv);
static void tim3_prepare_scan_mode(void);
static uint32_t measure_comp_frequency_hz(uint16_t dac_code);
static void tim3_apply_dynamic_divider(uint32_t input_freq_hz);
static void auto_tune_comp_and_divider(void);
static uint32_t fft_freq_to_bin(uint32_t freq_hz);
static float fft_bin_to_freq(uint32_t bin);
static float refine_peak_frequency(uint32_t peak_bin);
static void find_signal_peaks(float *freq_a_hz, float *mag_a, float *freq_b_hz, float *mag_b);
static void process_adc_fft_and_regenerate(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void uart_send_string(char *str)
{
	while (*str)
	{
		HAL_UART_Transmit(&huart1, (uint8_t *)str, 1U, 0xFFFFU);
		str++;
	}
}

void System_Init(void)
{
	MX_USART1_UART_Init();
}

static void tim3_div10_output_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	HAL_GPIO_DeInit(GPIOD, GPIO_PIN_12);
	GPIO_InitStruct.Pin = GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
	pd12_div_state = 0U;
	uart_send_string("BOOT: TIM3 div10 gpio ready\r\n");

	if (HAL_TIMEx_TISelection(&htim3, TIM_TIM3_TI2_COMP1, TIM_CHANNEL_2) != HAL_OK)
	{
		uart_send_string("ERR: HAL_TIMEx_TISelection failed\r\n");
		Error_Handler();
	}
	uart_send_string("BOOT: TIM3 TISelection ok\r\n");
	if (HAL_TIM_IC_Start(&htim3, TIM_CHANNEL_2) != HAL_OK)
	{
		uart_send_string("ERR: HAL_TIM_IC_Start failed\r\n");
		Error_Handler();
	}
	uart_send_string("BOOT: TIM3 IC start ok\r\n");
	if (HAL_TIM_Base_Start(&htim3) != HAL_OK)
	{
		uart_send_string("ERR: HAL_TIM_Base_Start failed\r\n");
		Error_Handler();
	}
	uart_send_string("BOOT: TIM3 base start ok\r\n");
	__HAL_TIM_DISABLE_IT(&htim3, TIM_IT_UPDATE);
}

static void format_float_3(char *buffer, size_t buffer_size, float value)
{
	long scaled;
	long integer_part;
	long frac_part;

	if (buffer_size == 0U)
	{
		return;
	}

	scaled = lroundf(value * 1000.0f);
	integer_part = scaled / 1000L;
	frac_part = labs(scaled % 1000L);
	snprintf(buffer, buffer_size, "%ld.%03ld", integer_part, frac_part);
}

static uint16_t dac_code_from_mv(uint32_t mv)
{
	uint32_t code = ((mv * 4095U) + 1650U) / 3300U;
	if (code > 4095U)
	{
		code = 4095U;
	}
	return (uint16_t)code;
}

static void tim3_prepare_scan_mode(void)
{
	__HAL_TIM_DISABLE_IT(&htim3, TIM_IT_UPDATE);
	__HAL_TIM_SET_AUTORELOAD(&htim3, 0xFFFFU);
	__HAL_TIM_SET_COUNTER(&htim3, 0U);
	__HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_UPDATE);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
	pd12_div_state = 0U;
	tim3_update_count = 0U;
	tim3_update_count_last = 0U;
}

static uint32_t measure_comp_frequency_hz(uint16_t dac_code)
{
	uint32_t start_tick;
	uint32_t edge_count;

	HAL_DAC_SetValue(&hdac3, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_code);
	HAL_Delay(20U);
	tim3_prepare_scan_mode();
	start_tick = HAL_GetTick();
	while ((HAL_GetTick() - start_tick) < COMP_SCAN_WINDOW_MS)
	{
	}
	edge_count = __HAL_TIM_GET_COUNTER(&htim3);
	return edge_count * (1000U / COMP_SCAN_WINDOW_MS);
}

static void tim3_apply_dynamic_divider(uint32_t input_freq_hz)
{
	uint32_t divider_edges;

	divider_edges = (input_freq_hz + COMP_TARGET_OUT_HZ) / (2U * COMP_TARGET_OUT_HZ);
	if (divider_edges < 1U)
	{
		divider_edges = 1U;
	}
	if (divider_edges > 65535U)
	{
		divider_edges = 65535U;
	}

	tim3_divider_edges = divider_edges;
	comp_input_freq_hz = input_freq_hz;
	__HAL_TIM_DISABLE_IT(&htim3, TIM_IT_UPDATE);
	__HAL_TIM_SET_AUTORELOAD(&htim3, divider_edges - 1U);
	__HAL_TIM_SET_COUNTER(&htim3, 0U);
	__HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_UPDATE);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
	pd12_div_state = 0U;
	tim3_update_count = 0U;
	tim3_update_count_last = 0U;
	__HAL_TIM_ENABLE_IT(&htim3, TIM_IT_UPDATE);
}

static void auto_tune_comp_and_divider(void)
{
	uint32_t mv;
	uint32_t best_freq = 0U;
	uint16_t best_code = comp_dac_code;
	uint32_t best_error = 0xFFFFFFFFUL;
	char line[160];

	uart_send_string("COMP auto scan start\r\n");
	for (mv = COMP_SCAN_START_MV; mv <= COMP_SCAN_END_MV; mv += COMP_SCAN_STEP_MV)
	{
		uint16_t code = dac_code_from_mv(mv);
		uint32_t measured_freq = measure_comp_frequency_hz(code);
		uint32_t divider_edges = (measured_freq + COMP_TARGET_OUT_HZ) / (2U * COMP_TARGET_OUT_HZ);
		uint32_t estimated_out_hz;
		uint32_t error_hz;

		if (divider_edges < 1U)
		{
			divider_edges = 1U;
		}
		estimated_out_hz = measured_freq / (2U * divider_edges);
		error_hz = (estimated_out_hz > COMP_TARGET_OUT_HZ) ? (estimated_out_hz - COMP_TARGET_OUT_HZ) : (COMP_TARGET_OUT_HZ - estimated_out_hz);

		sprintf(line,
				"COMP scan: %lumV code=%u, comp=%luHz, div=%lu, out=%luHz, err=%luHz\r\n",
				(unsigned long)mv,
				code,
				(unsigned long)measured_freq,
				(unsigned long)divider_edges,
				(unsigned long)estimated_out_hz,
				(unsigned long)error_hz);
		uart_send_string(line);

		if (measured_freq > 0U && error_hz < best_error)
		{
			best_error = error_hz;
			best_code = code;
			best_freq = measured_freq;
		}
	}

	comp_dac_code = best_code;
	comp_best_error_hz = best_error;
	HAL_DAC_SetValue(&hdac3, DAC_CHANNEL_1, DAC_ALIGN_12B_R, comp_dac_code);
	tim3_apply_dynamic_divider(best_freq);

	sprintf(line,
			"COMP scan selected: code=%u, comp=%luHz, div=%lu, target=%luHz, err=%luHz\r\n",
			comp_dac_code,
			(unsigned long)comp_input_freq_hz,
			(unsigned long)tim3_divider_edges,
			(unsigned long)COMP_TARGET_OUT_HZ,
			(unsigned long)comp_best_error_hz);
	uart_send_string(line);
}

static uint32_t fft_freq_to_bin(uint32_t freq_hz)
{
	uint32_t bin = (uint32_t)((((uint64_t)freq_hz) * FFT_LENGTH + ((uint64_t)ADC1_SAMPLE_FREQ_HZ / 2ULL)) / (uint64_t)ADC1_SAMPLE_FREQ_HZ);
	uint32_t max_bin = (FFT_LENGTH / 2U) - 1U;
	if (bin > max_bin)
	{
		bin = max_bin;
	}
	return bin;
}

static float fft_bin_to_freq(uint32_t bin)
{
	return ((float)bin * ADC1_SAMPLE_FREQ_HZ) / (float)FFT_LENGTH;
}

static float refine_peak_frequency(uint32_t peak_bin)
{
	float left;
	float center;
	float right;
	float denom;
	float delta = 0.0f;

	if ((peak_bin == 0U) || (peak_bin >= ((FFT_LENGTH / 2U) - 1U)))
	{
		return fft_bin_to_freq(peak_bin);
	}

	left = fft_out[peak_bin - 1U];
	center = fft_out[peak_bin];
	right = fft_out[peak_bin + 1U];
	denom = left - (2.0f * center) + right;
	if (fabsf(denom) > 1e-6f)
	{
		delta = 0.5f * (left - right) / denom;
		if (delta > 1.0f)
		{
			delta = 1.0f;
		}
		else if (delta < -1.0f)
		{
			delta = -1.0f;
		}
	}
	return (((float)peak_bin + delta) * ADC1_SAMPLE_FREQ_HZ) / (float)FFT_LENGTH;
}

static void find_signal_peaks(float *freq_a_hz, float *mag_a, float *freq_b_hz, float *mag_b)
{
	uint32_t min_bin = fft_freq_to_bin(FFT_SEARCH_FREQ_MIN_HZ);
	uint32_t max_bin = fft_freq_to_bin(FFT_SEARCH_FREQ_MAX_HZ);
	uint32_t peak1_bin = min_bin;
	uint32_t peak2_bin = min_bin;
	float peak1_mag = 0.0f;
	float peak2_mag = 0.0f;
	uint32_t i;
	uint32_t exclusion_radius = 3U;

	for (i = min_bin; i <= max_bin; i++)
	{
		if (fft_out[i] > peak1_mag)
		{
			peak1_mag = fft_out[i];
			peak1_bin = i;
		}
	}

	for (i = min_bin; i <= max_bin; i++)
	{
		uint32_t distance = (i > peak1_bin) ? (i - peak1_bin) : (peak1_bin - i);
		if (distance <= exclusion_radius)
		{
			continue;
		}
		if (fft_out[i] > peak2_mag)
		{
			peak2_mag = fft_out[i];
			peak2_bin = i;
		}
	}

	*freq_a_hz = refine_peak_frequency(peak1_bin);
	*mag_a = peak1_mag;
	*freq_b_hz = refine_peak_frequency(peak2_bin);
	*mag_b = peak2_mag;
	if (*freq_a_hz > *freq_b_hz)
	{
		float tmp_freq = *freq_a_hz;
		float tmp_mag = *mag_a;
		*freq_a_hz = *freq_b_hz;
		*mag_a = *mag_b;
		*freq_b_hz = tmp_freq;
		*mag_b = tmp_mag;
	}
}

static void process_adc_fft_and_regenerate(void)
{
	float local_avg = 0.0f;
	float local_min = adc1_data[0];
	float local_max = adc1_data[0];
	float freq_a = 0.0f;
	float freq_b = 0.0f;
	float mag_a = 0.0f;
	float mag_b = 0.0f;
	uint32_t i;

	for (i = 0U; i < ADC1_DMA_BUFFER_LENGTH; i++)
	{
		float value = adc1_data[i];
		local_avg += value;
		if (value < local_min)
		{
			local_min = value;
		}
		if (value > local_max)
		{
			local_max = value;
		}
	}
	local_avg /= (float)ADC1_DMA_BUFFER_LENGTH;

	calculate_fft(adc1_data, FFT_LENGTH);
	find_signal_peaks(&freq_a, &mag_a, &freq_b, &mag_b);

	adc_avg_v = local_avg;
	adc_min_v = local_min;
	adc_max_v = local_max;
	signal_a_hz = freq_a;
	signal_b_hz = freq_b;
	signal_a_mag = mag_a;
	signal_b_mag = mag_b;

	dds_update_frequency(0U, (uint32_t)(freq_a + 0.5f));
	dds_update_frequency(1U, (uint32_t)(freq_b + 0.5f));

	if (HAL_ADC_Start_DMA(&hadc1, adc1_dma_buffer, ADC1_DMA_BUFFER_LENGTH) != HAL_OK)
	{
		uart_send_string("ERR: HAL_ADC_Start_DMA rearm failed\r\n");
	}
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
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_COMP1_Init();
  MX_DAC3_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_SPI3_Init();
  /* USER CODE BEGIN 2 */
	System_Init();
	uart_send_string("BOOT: after System_Init\r\n");
	HAL_DAC_SetValue(&hdac3, DAC_CHANNEL_1, DAC_ALIGN_12B_R, comp_dac_code);
	HAL_DAC_Start(&hdac3, DAC_CHANNEL_1);
	uart_send_string("BOOT: after DAC start\r\n");
	if (HAL_COMP_Start(&hcomp1) != HAL_OK)
	{
		Error_Handler();
	}
	uart_send_string("BOOT: after COMP start\r\n");
	HAL_NVIC_DisableIRQ(COMP1_2_3_IRQn);
	{
		char line[80];
		sprintf(line, "COMP DAC start code: %u\r\n", comp_dac_code);
		uart_send_string(line);
	}
	uart_send_string("BOOT: before TIM3 div10 init\r\n");
	tim3_div10_output_init();
	uart_send_string("BOOT: after TIM3 div10 init\r\n");
	auto_tune_comp_and_divider();
	uart_send_string("TIM3 dynamic divided output started on PD12\r\n");
	uart_send_string("LCD init skipped for debug\r\n");
	init_hanning_window();
	My_ADC_Init();
	AD9833_Init();
	dds_proc();
	uart_send_string("FFT + AD9833 chain started\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
		static uint32_t last_report_ms = 0U;
		adc1_deal();
		if (adc1_proc_flag)
		{
			adc1_proc_flag = 0U;
			process_adc_fft_and_regenerate();
		}
		if ((HAL_GetTick() - last_report_ms) >= 500U)
		{
			char line[160];
			char freq_str[24];
			char avg_str[24];
			char min_str[24];
			char max_str[24];
			char sig_a_str[24];
			char sig_b_str[24];
			char mag_a_str[24];
			char mag_b_str[24];
			uint32_t update_delta = tim3_update_count - tim3_update_count_last;
			float divided_freq_hz = (float)update_delta;
			last_report_ms = HAL_GetTick();
			tim3_update_count_last = tim3_update_count;
			format_float_3(freq_str, sizeof(freq_str), divided_freq_hz);
			format_float_3(avg_str, sizeof(avg_str), adc_avg_v);
			format_float_3(min_str, sizeof(min_str), adc_min_v);
			format_float_3(max_str, sizeof(max_str), adc_max_v);
			format_float_3(sig_a_str, sizeof(sig_a_str), signal_a_hz);
			format_float_3(sig_b_str, sizeof(sig_b_str), signal_b_hz);
			format_float_3(mag_a_str, sizeof(mag_a_str), signal_a_mag);
			format_float_3(mag_b_str, sizeof(mag_b_str), signal_b_mag);
			sprintf(line,
					"COMP auto mode: dac=%u, comp=%luHz, div=%lu, err=%luHz, updates/500ms=%lu, PD12=%sHz\r\n",
					comp_dac_code,
					(unsigned long)comp_input_freq_hz,
					(unsigned long)tim3_divider_edges,
					(unsigned long)comp_best_error_hz,
					update_delta,
					freq_str);
			uart_send_string(line);
			sprintf(line, "ADC Avg: %s V\r\n", avg_str);
			uart_send_string(line);
			sprintf(line, "ADC Min/Max: %s / %s V\r\n", min_str, max_str);
			uart_send_string(line);
			sprintf(line, "Signal A': %s Hz, Mag: %s\r\n", sig_a_str, mag_a_str);
			uart_send_string(line);
			sprintf(line, "Signal B': %s Hz, Mag: %s\r\n", sig_b_str, mag_b_str);
			uart_send_string(line);
		}
	}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV2;
  RCC_OscInitStruct.PLL.PLLN = 25;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_COMP_TriggerCallback(COMP_HandleTypeDef *hcomp)
{
	(void)hcomp;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	(void)htim;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM3)
	{
		pd12_div_state ^= 1U;
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, pd12_div_state ? GPIO_PIN_SET : GPIO_PIN_RESET);
		tim3_update_count++;
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
