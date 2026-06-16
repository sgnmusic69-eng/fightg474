#include "dds_apl.h"

WaveformConfig_t WaveformConfig[2];

/*
 * Common dual-tone test pairs.
 * Keep A < B and stay within the usual 20 kHz ~ 100 kHz band.
 */
static const DdsFreqPair_t k_test_pairs[] =
{
	{ 20000U,  40000U },
	{ 20000U,  60000U },
	{ 25000U,  50000U },
	{ 25000U,  75000U },
	{ 30000U,  60000U },
	{ 30000U,  90000U },
	{ 35000U,  70000U },
	{ 40000U,  80000U },
	{ 45000U,  90000U },
	{ 50000U, 100000U }
};

static void dds_apply_pair(const DdsFreqPair_t *pair)
{
	WaveformConfig[0].ch = AD9833_CH1;
	WaveformConfig[0].type = AD9833_Sinusoid;
	WaveformConfig[0].frequency = pair->freq_a_hz;
	waveset(WaveformConfig[0].ch, WaveformConfig[0].type, WaveformConfig[0].frequency);

	WaveformConfig[1].ch = AD9833_CH2;
	WaveformConfig[1].type = AD9833_Sinusoid;
	WaveformConfig[1].frequency = pair->freq_b_hz;
	waveset(WaveformConfig[1].ch, WaveformConfig[1].type, WaveformConfig[1].frequency);
}

void dds_proc(void)
{
	static uint8_t configured = 0U;

	if (configured)
	{
		return;
	}

	dds_apply_pair(&k_test_pairs[0]);
	configured = 1U;
}

void dds_update_frequency(uint8_t ch, uint32_t freq)
{
	if (ch < 2U)
	{
		WaveformConfig[ch].frequency = freq;
		waveset(WaveformConfig[ch].ch, WaveformConfig[ch].type, WaveformConfig[ch].frequency);
	}
}

void dds_set_test_pair(uint8_t pair_index)
{
	uint8_t pair_count = (uint8_t)(sizeof(k_test_pairs) / sizeof(k_test_pairs[0]));

	if (pair_count == 0U)
	{
		return;
	}

	if (pair_index >= pair_count)
	{
		pair_index = (uint8_t)(pair_index % pair_count);
	}

	dds_apply_pair(&k_test_pairs[pair_index]);
}

uint8_t dds_get_test_pair_count(void)
{
	return (uint8_t)(sizeof(k_test_pairs) / sizeof(k_test_pairs[0]));
}

const DdsFreqPair_t *dds_get_test_pair(uint8_t pair_index)
{
	uint8_t pair_count = dds_get_test_pair_count();

	if ((pair_count == 0U) || (pair_index >= pair_count))
	{
		return 0;
	}

	return &k_test_pairs[pair_index];
}
