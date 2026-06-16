#ifndef __DDS_APL_H__
#define __DDS_APL_H__

#include "main.h"

typedef struct
{
	uint32_t freq_a_hz;
	uint32_t freq_b_hz;
} DdsFreqPair_t;

void dds_proc(void);
void dds_update_frequency(uint8_t ch, uint32_t freq);
void dds_set_test_pair(uint8_t pair_index);
uint8_t dds_get_test_pair_count(void);
const DdsFreqPair_t *dds_get_test_pair(uint8_t pair_index);
#endif
