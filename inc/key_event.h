#ifndef __KEY_EVENT_H__
#define __KEY_EVENT_H__

#include "ti_msp_dl_config.h"

#define EVENTBUFFERSIZE 5

typedef struct
{
    // buffer to storage btnEvent
    // use 8 bit to store both button and event
    //      button   |event
    // LSB  xxxx     |xxxx    MSB
    uint8_t btnEventBuf[EVENTBUFFERSIZE];
    uint8_t bufsize;
    uint8_t front;
    uint8_t rear;
} menu_event;

typedef enum
{
    USER_BUTTON_0 = 0,
    USER_BUTTON_MAX,
} user_button_t;

void user_button_init(void);
uint8_t get_earlest_event(void);

#endif
