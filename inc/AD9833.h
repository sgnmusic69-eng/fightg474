#ifndef __AD9833_H__
#define __AD9833_H__

#include "main.h"

// #define FMCLK 25000000 // Master Clock On AD9833
#define FMCLK 51200000 // Master Clock On AD9833

typedef enum
{
    AD9833_WAVE_SINE = 0,
    AD9833_WAVE_SQUARE,
    AD9833_WAVE_TRIANGLE
} ad9833_wave_t; // Wave Selection Enum

typedef struct
{
    SPI_HandleTypeDef  *hspi;
    GPIO_TypeDef       *fsync_port;
    uint16_t           fsync_pin;
    uint32_t           mclk_hz;
} ad9833_t;

void AD9833_Select(ad9833_t *dev);
void AD9833_Deselect(ad9833_t *dev);
HAL_StatusTypeDef AD9833_Write16(ad9833_t *dev, uint16_t word);

HAL_StatusTypeDef AD9833_SetWave      (ad9833_t *dev, ad9833_wave_t wave);
HAL_StatusTypeDef AD9833_SetFreqPhase (ad9833_t *dev, ad9833_wave_t wave, float freq_hz, float phase_deg);
HAL_StatusTypeDef AD9833_Init         (ad9833_t *dev, ad9833_wave_t wave, float freq_hz, float phase_deg);

#endif
