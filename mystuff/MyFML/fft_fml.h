#ifndef __FFT_FML_H__
#define __FFT_FML_H__

#include "main.h"
#include "stdio.h"
#include "string.h"
#include "arm_math.h"

void init_hanning_window(void);
void calculate_fft(float* input_data, uint32_t data_len);

#endif

