#include "fft_apl.h"

uint8_t fft_proc_flag = 0;
void fft_proc(void)
{
	if(fft_proc_flag)
	{
		fft_proc_flag = 0;
		adc1_fft_deal();//24
	  //Usart_Send_ADC_Data(fft_out, FFT_LENGTH / 2);
		
	}
}
