#include "fft_fml.h"
#include <math.h>

float fft_input_buffer[FFT_LENGTH * 2];
float fft_magnitude[FFT_LENGTH];
float window_buffer[FFT_LENGTH];
float fft_out[FFT_LENGTH / 2];

void init_hanning_window(void)
{
	for (int i = 0; i < FFT_LENGTH; i++)
	{
		window_buffer[i] = 0.5f * (1.0f - cos(2.0f * PI * i / (FFT_LENGTH - 1)));
	}
}

void calculate_fft(float* input_data, uint32_t data_len)
{
	uint16_t i;
	uint16_t actual_length = data_len;
	float dc_mean = 0.0f;

	memset((char *)fft_input_buffer, 0, sizeof(fft_input_buffer));
	memset((char *)fft_magnitude, 0, sizeof(fft_magnitude));
	memset((char *)fft_out, 0, sizeof(fft_out));

	for (i = 0; i < actual_length; i++)
	{
		dc_mean += input_data[i];
	}
	dc_mean /= actual_length;

	/* Remove DC before windowing to reduce leakage into the whole band. */
	for (i = 0; i < actual_length; i++)
	{
		float sample = input_data[i] - dc_mean;
		fft_input_buffer[2 * i] = sample * window_buffer[i];
		fft_input_buffer[2 * i + 1] = 0.0f;
	}

	arm_cfft_f32(&arm_cfft_sR_f32_len2048, fft_input_buffer, 0, 1);
	arm_cmplx_mag_f32(fft_input_buffer, fft_magnitude, FFT_LENGTH);

	for (i = 0; i < FFT_LENGTH / 2; i++)
	{
		fft_out[i] = fft_magnitude[i];
	}
}
