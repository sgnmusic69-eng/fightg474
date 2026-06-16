#include "my_spi.h"

void AD9833_SPI_16bit_Write(uint16_t data)
{
	uint16_t send_data = data;
	for (int i = 0; i < 16; i++)
	{
		if (send_data & 0x8000U)
		{
			AD9833_SPI_SDA_H;
		}
		else
		{
			AD9833_SPI_SDA_L;
		}

		AD9833_SPI_SCK_L;
		__NOP();
		__NOP();
		AD9833_SPI_SCK_H;
		__NOP();
		__NOP();

		send_data <<= 1;
	}
}
