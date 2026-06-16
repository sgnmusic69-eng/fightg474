#include "adc_fml.h" 


uint32_t adc1_dma_buffer[ADC1_DMA_BUFFER_LENGTH]; 
uint8_t adc1_deal_flag = 0;
void My_ADC_Init(void)
{
	HAL_TIM_Base_Start(&htim1);
	HAL_ADC_Start_DMA(&hadc1, adc1_dma_buffer, ADC1_DMA_BUFFER_LENGTH);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc->Instance == ADC1)
	{
		adc1_deal_flag = 1;
	}
}
