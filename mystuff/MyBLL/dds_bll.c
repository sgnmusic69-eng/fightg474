#include "dds_bll.h"

void waveset(uint16_t ch, uint16_t Type, uint32_t Frequency)
{	
	AD9833_Set_Frequency(ch, Type, Frequency);

}
