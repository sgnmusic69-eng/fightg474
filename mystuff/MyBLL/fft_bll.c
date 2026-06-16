#include "fft_bll.h"
#include "main.h"




void adc1_fft_deal(void)
{
	calculate_fft(adc1_data, FFT_LENGTH);
}
