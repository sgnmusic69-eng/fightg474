#include "AD9833.h"

#include <stddef.h>
#include <stdint.h>

#define AD9833_CTRL_B28           0x2000U
#define AD9833_CTRL_RESET         0x0100U
#define AD9833_CTRL_TRIANGLE      0x0002U
#define AD9833_CTRL_SQUARE        0x0028U
#define AD9833_FREQ0_REG          0x4000U
#define AD9833_PHASE0_REG         0xC000U
#define AD9833_FREQ_WORD_MASK     0x3FFFU
#define AD9833_PHASE_WORD_MASK    0x0FFFU
#define AD9833_MAX_TUNING_WORD    0x0FFFFFFFU
#define AD9833_TUNING_WORD_SCALE  268435456.0

static uint16_t AD9833_GetWaveControl(ad9833_wave_t wave)
{
    switch (wave)
    {
    case AD9833_WAVE_SQUARE:
        return AD9833_CTRL_SQUARE;

    case AD9833_WAVE_TRIANGLE:
        return AD9833_CTRL_TRIANGLE;

    case AD9833_WAVE_SINE:
    default:
        return 0x0000U;
    }
}

static uint32_t AD9833_GetMclkHz(const ad9833_t *dev)
{
    if ((dev != NULL) && (dev->mclk_hz != 0U))
    {
        return dev->mclk_hz;
    }

    return FMCLK;
}

void AD9833_Select(ad9833_t *dev)
{
    HAL_GPIO_WritePin(dev->fsync_port, dev->fsync_pin, GPIO_PIN_RESET);
}

void AD9833_Deselect(ad9833_t *dev)
{
    HAL_GPIO_WritePin(dev->fsync_port, dev->fsync_pin, GPIO_PIN_SET);
}

HAL_StatusTypeDef AD9833_Write16(ad9833_t *dev, uint16_t word)
{
    uint8_t tx[2];

    if ((dev == NULL) || (dev->hspi == NULL) || (dev->fsync_port == NULL))
    {
        return HAL_ERROR;
    }

    tx[0] = (uint8_t)((word >> 8) & 0xFFU);
    tx[1] = (uint8_t)(word & 0xFFU);

    AD9833_Select(dev);
    HAL_StatusTypeDef ret = HAL_SPI_Transmit(dev->hspi, tx, 2U, HAL_MAX_DELAY);
    AD9833_Deselect(dev);

    return ret;
}

HAL_StatusTypeDef AD9833_SetWave(ad9833_t *dev, ad9833_wave_t wave)
{
    uint16_t control_word = AD9833_CTRL_B28 | AD9833_GetWaveControl(wave);
    return AD9833_Write16(dev, control_word);
}

HAL_StatusTypeDef AD9833_SetFreqPhase(ad9833_t *dev, ad9833_wave_t wave, float freq_hz, float phase_deg)
{
    uint32_t mclk_hz;
    uint32_t tuning_word;
    uint16_t freq_low;
    uint16_t freq_high;
    uint16_t phase_word;
    uint16_t control_word;
    HAL_StatusTypeDef ret;

    if ((dev == NULL) || (dev->hspi == NULL) || (dev->fsync_port == NULL))
    {
        return HAL_ERROR;
    }

    if (freq_hz < 0.0f)
    {
        freq_hz = 0.0f;
    }

    if (phase_deg < 0.0f)
    {
        phase_deg = 0.0f;
    }
    else if (phase_deg > 360.0f)
    {
        phase_deg = 360.0f;
    }

    mclk_hz = AD9833_GetMclkHz(dev);
    tuning_word = (uint32_t)((freq_hz * AD9833_TUNING_WORD_SCALE) / (float)mclk_hz);
    if (tuning_word > AD9833_MAX_TUNING_WORD)
    {
        tuning_word = AD9833_MAX_TUNING_WORD;
    }

    freq_low = (uint16_t)(AD9833_FREQ0_REG | (tuning_word & AD9833_FREQ_WORD_MASK));
    freq_high = (uint16_t)(AD9833_FREQ0_REG | ((tuning_word >> 14) & AD9833_FREQ_WORD_MASK));
    phase_word = (uint16_t)(AD9833_PHASE0_REG |
                            (((uint16_t)(phase_deg * (4096.0f / 360.0f))) & AD9833_PHASE_WORD_MASK));
    control_word = (uint16_t)(AD9833_CTRL_B28 | AD9833_GetWaveControl(wave));

    ret = AD9833_Write16(dev, (uint16_t)(control_word | AD9833_CTRL_RESET));
    if (ret != HAL_OK)
    {
        return ret;
    }

    ret = AD9833_Write16(dev, freq_low);
    if (ret != HAL_OK)
    {
        return ret;
    }

    ret = AD9833_Write16(dev, freq_high);
    if (ret != HAL_OK)
    {
        return ret;
    }

    ret = AD9833_Write16(dev, phase_word);
    if (ret != HAL_OK)
    {
        return ret;
    }

    return AD9833_Write16(dev, control_word);
}

HAL_StatusTypeDef AD9833_Init(ad9833_t *dev, ad9833_wave_t wave, float freq_hz, float phase_deg)
{
    return AD9833_SetFreqPhase(dev, wave, freq_hz, phase_deg);
}
