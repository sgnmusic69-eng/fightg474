#include "freqDiv.h"

#include <stdlib.h>
#include <string.h>

#define DAC12_REF_VOLTAGE_mV (2500)
#define DAC12_OUTPUT_VOLTAGE_mV_MIN (500)
#define DAC12_OUTPUT_VOLTAGE_mV_MAX (2100)

uint8_t pos = 0;
uint8_t div_factor = 0;
volatile uint16_t gpio_counter = 0;
uint16_t counterBuffer[5];
volatile bool testflag = false, divflag = false, countMode = false;
float dacOutputVoltage = 0;

static int cmp_int(const void *a, const void *b)
{
    uint16_t ia = *(const uint16_t *)a;
    uint16_t ib = *(const uint16_t *)b;
    return (ia < ib) ? -1 : (ia > ib);
}

bool isStableAndMultiple250(const uint16_t counterBuf[5], int threshold)
{
    uint16_t tmp[5];
    memcpy(tmp, counterBuf, sizeof(tmp));
    qsort(tmp, 5, sizeof(uint16_t), cmp_int);
    uint16_t median = tmp[2];
    for (uint8_t i = 0; i < 5; ++i)
    {
        if (abs(counterBuf[i] - median) > threshold)
        {
            return false;
        }
    }

    int r = median % 250;
    if (r > 125)
        r -= 250;
    else if (r < -125)
        r += 250;
    return (abs(r) <= threshold) && (median / 250 > 1);
}

void doTestFreq(void)
{
    countMode = false;
    uint32_t DAC_value;
    bool judgeResult;
    gpio_counter = 0;
    DAC_value = (DAC12_OUTPUT_VOLTAGE_mV_MIN * 4095) / DAC12_REF_VOLTAGE_mV;

    DL_DAC12_output12(DAC0, DAC_value);
    DL_DAC12_enable(DAC0);
    memset(counterBuffer, 0, sizeof(counterBuffer));
    DL_TimerA_stopCounter(COMPARE_DIV_INST);
    delay_cycles(8000);
    for (uint16_t i = DAC12_OUTPUT_VOLTAGE_mV_MIN;
         i < DAC12_OUTPUT_VOLTAGE_mV_MAX; i += 50)
    {
        DAC_value = (i * 4095) / DAC12_REF_VOLTAGE_mV;

        DL_DAC12_output12(DAC0, DAC_value);
        DL_DAC12_enable(DAC0);

        NVIC_EnableIRQ(TIMER_FREQ_INST_INT_IRQN);
        testflag = false;
        pos = 0;
        gpio_counter = 0;
        NVIC_EnableIRQ(GPIO_DIV_INT_IRQN);
        DL_GPIO_enableInterrupt(GPIOB, GPIO_DIV_PIN_3_PIN);
        DL_TimerG_startCounter(TIMER_FREQ_INST);

        while (testflag == false)
            ;

        DL_TimerG_stopCounter(TIMER_FREQ_INST);
        NVIC_DisableIRQ(GPIO_DIV_INT_IRQN);
        DL_GPIO_disableInterrupt(GPIOB, GPIO_DIV_PIN_3_PIN);
        NVIC_DisableIRQ(TIMER_FREQ_INST_INT_IRQN);
        judgeResult = isStableAndMultiple250(counterBuffer, 15);
        if (judgeResult == true)
        {
            div_factor = counterBuffer[2] / 250;
            DL_TimerA_CompareConfig gCOMPARE_DIVCompareConfig = {
                .compareMode = DL_TIMER_COMPARE_MODE_EDGE_COUNT,
                .count = (div_factor - 1),
                .startTimer = DL_TIMER_STOP,
                .edgeDetectMode = DL_TIMER_COMPARE_EDGE_DETECTION_MODE_EDGE,
                .inputChan = DL_TIMER_INPUT_CHAN_1,
                .inputInvMode = DL_TIMER_CC_INPUT_INV_NOINVERT,
            };
            DL_TimerA_initCompareMode(
                COMPARE_DIV_INST,
                (DL_TimerA_CompareConfig *)&gCOMPARE_DIVCompareConfig);
            DL_TimerA_setCaptureCompareInputFilter(
                COMPARE_DIV_INST, DL_TIMER_CC_INPUT_FILT_CPV_CONSEC_PER,
                DL_TIMER_CC_INPUT_FILT_FP_PER_8, DL_TIMER_CC_1_INDEX);
            DL_TimerA_enableCaptureCompareInputFilter(COMPARE_DIV_INST,
                                                      DL_TIMER_CC_1_INDEX);

            DL_TimerA_enableClock(COMPARE_DIV_INST);
            DL_TimerA_enableEvent(COMPARE_DIV_INST, DL_TIMERA_EVENT_ROUTE_1,
                                  (DL_TIMERA_EVENT_LOAD_EVENT));

            DL_TimerA_setPublisherChanID(COMPARE_DIV_INST,
                                         DL_TIMERA_PUBLISHER_INDEX_0,
                                         COMPARE_DIV_INST_PUB_0_CH);
            DL_TimerA_startCounter(COMPARE_DIV_INST);
            divflag = true;
            break;
        }
    }
}

bool ifLocked(void)
{
    countMode = true;
    uint32_t DAC_value;
    bool judgeResult;
    gpio_counter = 0;
    memset(counterBuffer, 0, sizeof(counterBuffer));
    NVIC_EnableIRQ(TIMER_FREQ_INST_INT_IRQN);
    testflag = false;
    pos = 0;
    gpio_counter = 0;
    NVIC_EnableIRQ(GPIO_DIV_INT_IRQN);
    DL_GPIO_enableInterrupt(GPIOB, GPIO_DIV_PIN_5_PIN);
    DL_TimerG_startCounter(TIMER_FREQ_INST);

    while (testflag == false)
        ;

    DL_TimerG_stopCounter(TIMER_FREQ_INST);
    DL_GPIO_disableInterrupt(GPIOB, GPIO_DIV_PIN_5_PIN);
    NVIC_DisableIRQ(GPIO_DIV_INT_IRQN);
    NVIC_DisableIRQ(TIMER_FREQ_INST_INT_IRQN);
    judgeResult = isStableAndMultiple250(counterBuffer, 2);
    if (judgeResult == true && counterBuffer[2] / 250 == 2)
    {
        return true;
    }
    return false;
}

void GROUP1_IRQHandler(void)
{
    uint32_t pendingGPIO = DL_GPIO_getEnabledInterruptStatus(
        GPIO_DIV_PORT, GPIO_DIV_PIN_3_PIN | GPIO_DIV_PIN_5_PIN);
    if (pendingGPIO & GPIO_DIV_PIN_3_PIN)
    {
        if (DL_GPIO_readPins(GPIO_DIV_PORT, GPIO_DIV_PIN_3_PIN))
            gpio_counter++;
    }
    if (pendingGPIO & GPIO_DIV_PIN_5_PIN)
    {
        if (DL_GPIO_readPins(GPIO_DIV_PORT, GPIO_DIV_PIN_5_PIN))
            gpio_counter++;
    }
}

void TIMER_FREQ_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(TIMER_FREQ_INST))
    {
    case DL_TIMER_IIDX_ZERO:
        counterBuffer[pos++] = gpio_counter;
        gpio_counter = 0;
        if (pos == 5)
            testflag = true;
        break;
    default:
        break;
    }
}
