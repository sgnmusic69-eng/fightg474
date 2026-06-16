#include "adc_bll.h"

uint8_t adc1_proc_flag = 0;
float adc1_data[ADC1_DMA_BUFFER_LENGTH];
void adc1_deal(void)
{
	if(adc1_deal_flag)
	{
		adc1_deal_flag = 0;
		for(int i=0;i<ADC1_DMA_BUFFER_LENGTH;i++)
		{
			adc1_data[i] = adc1_dma_buffer[i] * 3.3 / 4096;
		}
		adc1_proc_flag = 1;
	}
}
