#ifndef __DDS_BLL_H__
#define __DDS_BLL_H__

#include "main.h"

typedef struct {
	uint16_t ch;				//通道
	uint16_t type;			//波形
	uint32_t frequency; //频率
	uint16_t phase;     //相位
}	WaveformConfig_t;

void waveset(uint16_t ch, uint16_t Type, uint32_t Frequency);
#endif

