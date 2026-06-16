#ifndef __JUDGE_H__
#define __JUDGE_H__

#include "ti_msp_dl_config.h"

#include "stdint.h"

#define MAX_COMPONENTS      2
#define SNR_THRESHOLD_RATIO 0.007f
#define FUND_THRESHOLD      100000.0f
#define MIN_HARMS           3
#define MAX_HARM_CHECK      5
#define THD_THRESHOLD       0.10f

typedef enum
{
    WAVE_SINE,
    WAVE_TRIANGLE
} WaveType;

typedef struct
{
    WaveType type; // wave form
    uint32_t freq; // fundamental frequency
    float phase;
} Component;

uint8_t detect_components(float mag[], int NFFT, int Fs, Component comps[]);
float get_phase(float fftResult[], float f_target, int Fs, int NFFT);
void apply_hann_window(float x[], int N);
float calcPhaseShiftDeg(float freq_hz, float delay_units);

#endif
