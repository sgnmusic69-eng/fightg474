#include "judge.h"
#include "math.h"

#include "arm_math.h"

#define myPI 3.141592653589793

void apply_hann_window(float x[], int N)
{
    for (int n = 0; n < N; n++)
    {
        // hanning window: w[n] = 0.5 * (1 - cos(2πn/(N-1)))
        float w = 0.5f * (1.0f - arm_cos_f32(2.0f * myPI * n / (N - 1)));
        x[n] *= w;
    }
}

float fit_harmonic_slope(const float mag[], int NFFT, int Fs, int f0)
{
    float sum_x = 0, sum_y = 0, sum_x2 = 0, sum_xy = 0;
    int count = 0;
    for (int k = 1; k <= 2 * MAX_HARM_CHECK + 1; k += 2)
    {
        float fk = (float)f0 * k;
        if (fk > Fs / 2)
            break;
        int idx = (int)(fk * NFFT / Fs + 0.5);
        if (idx >= NFFT / 2)
            break;
        float a = mag[idx];
        if (a < 1e-6)
            continue;
        float x = log((float)k);
        float y = log(a);
        sum_x += x;
        sum_y += y;
        sum_x2 += x * x;
        sum_xy += x * y;
        count++;
    }
    if (count < MIN_HARMS)
        return 0;
    return (count * sum_xy - sum_x * sum_y) / (count * sum_x2 - sum_x * sum_x);
}

float calcPhaseShiftDeg(float freq_hz, float delay_units)
{
    float delay_sec = delay_units * 1e-6;
    float phase = 360.0 * freq_hz * delay_sec;
    phase = fmodf(phase, 360.0);
    return phase;
}

float get_phase(float fftResult[], float f_target, int Fs, int NFFT)
{
    float kf = f_target / Fs * NFFT;
    int32_t k = (int32_t)(kf + 0.5f);
    if (k < 0)
        k = 0;
    if (k >= NFFT)
        k = NFFT - 1;
    float re = fftResult[2 * k];
    float im = fftResult[2 * k + 1];
    return atan2f(im, re) * 180 / myPI;
}

typedef struct {
    float freq;
    float A1;
    WaveType type;
} Cand;

static inline float get_bin(const float mag[], int NFFT, int Fs, float freq) {
    int idx = (int)(freq * NFFT / Fs + 0.5f);
    int nbins = NFFT/2;
    return (idx >= 0 && idx < nbins) ? mag[idx] : 0.0f;
}

static WaveType compute_type(const float mag[], int NFFT, int Fs,
                             float f0, float rel_thresh, float A1_out[1])
{
    float A1 = get_bin(mag, NFFT, Fs, f0);
    *A1_out = A1;
    if (A1 < FUND_THRESHOLD || A1 < rel_thresh) {
        return WAVE_SINE;
    }
    float energy = 0.0f;
    int count = 0;
    for (int k = 3; k <= 2*MAX_HARM_CHECK+1; k += 2) {
        float fk = f0 * (float)k;
        if (fk >= Fs/2.0f) break;
        float Ak = get_bin(mag, NFFT, Fs, fk);
        if (Ak < rel_thresh) continue;
        energy += Ak*Ak;
        count++;
    }
    if (count < MIN_HARMS) return WAVE_SINE;
    float thd = sqrtf(energy) / A1;
    return (thd > THD_THRESHOLD) ? WAVE_TRIANGLE : WAVE_SINE;
}

uint8_t detect_components(float mag[], int NFFT, int Fs, Component comps[])
{
    int nbins = NFFT/2;
    // 1) 计算全局噪声门限
    float max_mag = 0.0f;
    for (int i = 1; i < nbins; i++) {
        if (mag[i] > max_mag) max_mag = mag[i];
    }
    float rel_thresh = max_mag * SNR_THRESHOLD_RATIO;

    // 2) 为所有候选频率计算 A1 与类型
    const int ncand = (100000 - 5000) / 5000 + 1;
    Cand list[ncand];
    int idx = 0;
    for (int f0 = 5000; f0 <= 100000; f0 += 5000) {
        float A1;
        WaveType type = compute_type(mag, NFFT, Fs, (float)f0, rel_thresh, &A1);
        list[idx].freq = (float)f0;
        list[idx].A1   = A1;
        list[idx].type = type;
        idx++;
    }

    // 3) 按 A1 从大到小排序（简单选择排序，因为 ncand 最多 20）
    for (int i = 0; i < MAX_COMPONENTS; i++) {
        int best = i;
        for (int j = i+1; j < ncand; j++) {
            if (list[j].A1 > list[best].A1) best = j;
        }
        // 交换
        Cand tmp = list[i];
        list[i] = list[best];
        list[best] = tmp;
    }

    // 4) 填充输出
    uint8_t out = 0;
    for (int i = 0; i < MAX_COMPONENTS; i++) {
        if (list[i].A1 < FUND_THRESHOLD) break;
        comps[out].freq = list[i].freq;
        comps[out].type = list[i].type;
        out++;
    }
    return out;
}
