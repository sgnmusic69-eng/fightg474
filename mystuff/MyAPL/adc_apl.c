#include "adc_apl.h"
//#include
void adc_proc(void)
{
	adc1_deal();
	if(adc1_proc_flag)
	{
		adc1_proc_flag = 0;
////		Usart_Send_ADC_Data(adc1_data, ADC1_DMA_BUFFER_LENGTH);
//		fft_proc_flag = 1;
	}
}
