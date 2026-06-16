#include "ad9833.h"
//只能自己用
static uint32_t AD9833_ConvertHzToFreqWord(uint32_t frequency_hz)
{
	/* FTW = fout * 2^28 / MCLK */
	uint64_t numerator = ((uint64_t)frequency_hz << 28);
	return (uint32_t)(numerator / AD9833_MCLK_HZ);
}

void AD9833_Init(void)
{
	AD9833_SPI_CS1_H;
	AD9833_SPI_CS2_H;
	AD9833_Write_Reg(AD9833_ALL, AD9833_Reg_Control | AD9833_Reg_Control_B28 | AD9833_Reg_Control_RESET);
	HAL_Delay(5);
}

void AD9833_Write_Reg(uint16_t ch, uint16_t reg_data)
{
	if(ch == AD9833_ALL)
	{
		AD9833_SPI_CS1_L;
		AD9833_SPI_CS2_L;
		AD9833_SPI_16bit_Write(reg_data);
		AD9833_SPI_CS1_H;
		AD9833_SPI_CS2_H;
	}
	else if(ch == AD9833_CH1)
	{
		AD9833_SPI_CS1_L;
		AD9833_SPI_16bit_Write(reg_data);
		AD9833_SPI_CS1_H;
	}
	else if(ch == AD9833_CH2)
	{
		AD9833_SPI_CS2_L;
		AD9833_SPI_16bit_Write(reg_data);
		AD9833_SPI_CS2_H;
	}
}

void AD9833_Set_Frequency(uint16_t ch, uint16_t Type, uint32_t frequency_hz)
{
	uint32_t freq_word = AD9833_ConvertHzToFreqWord(frequency_hz);
	uint16_t freq_l = (uint16_t)(freq_word & 0x3FFFU);
	uint16_t freq_h = (uint16_t)((freq_word >> 14) & 0x3FFFU);
	uint16_t ctrl_reset = (uint16_t)(AD9833_Reg_Control | AD9833_Reg_Control_B28 | AD9833_Reg_Control_RESET | Type);
	uint16_t ctrl_run = (uint16_t)(AD9833_Reg_Control | AD9833_Reg_Control_B28 | Type);

	/* Hold reset, program FREQ0 28-bit, then release reset to start output */
	AD9833_Write_Reg(ch, ctrl_reset);
	AD9833_Write_Reg(ch, (uint16_t)(AD9833_Reg_Freq0 | freq_l));
	AD9833_Write_Reg(ch, (uint16_t)(AD9833_Reg_Freq0 | freq_h));
	AD9833_Write_Reg(ch, ctrl_run);
}

void AD9833_Output_Sine(uint16_t ch, uint32_t frequency_hz)
{
	AD9833_Set_Frequency(ch, AD9833_Sinusoid, frequency_hz);
}

