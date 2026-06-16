#include "key_event.h"
#include "flexible_button.h"

menu_event EventBuffer;
static flex_button_t user_button[USER_BUTTON_MAX];

uint8_t common_btn_read(void *arg)
{
    uint8_t value = 0;
    uint32_t pin = GPIO_KEY_PORT->DIN31_0;
    flex_button_t *btn = (flex_button_t *)arg;
    switch (btn->id)
    {
    case USER_BUTTON_0:
        value = (pin & GPIO_KEY_PIN_4_PIN) ? 1 : 0;
        break;
    default:
        value = 0;
        break;
    }
    return value;
}

void common_btn_event(void *arg)
{
    flex_button_t *btn = (flex_button_t *)arg;
    // use event queue to process button event
    if ((EventBuffer.rear + 1) % EventBuffer.bufsize == EventBuffer.front)
    {
        return;
    }
    EventBuffer.btnEventBuf[EventBuffer.rear] = ((btn->event & 0x0f) << 4) | (btn->id & 0x0f);
    EventBuffer.rear = (EventBuffer.rear + 1) % EventBuffer.bufsize;
}

void user_button_init(void)
{
    uint8_t i = 0;
    for (i = 0; i < USER_BUTTON_MAX; i++)
    {
        user_button[i].id = i;
        user_button[i].usr_button_read = common_btn_read;
        user_button[i].cb = common_btn_event;
        user_button[i].pressed_logic_level = 0;
        user_button[i].short_press_start_tick = FLEX_MS_TO_SCAN_CNT(800);
        user_button[i].long_press_start_tick = FLEX_MS_TO_SCAN_CNT(3000);
        user_button[i].long_hold_start_tick = FLEX_MS_TO_SCAN_CNT(4500);

        flex_button_register(&user_button[i]);
    }
    EventBuffer.bufsize = EVENTBUFFERSIZE;
    EventBuffer.front = 0;
    EventBuffer.rear = 0;
}

uint8_t get_earlest_event(void)
{

    uint8_t temp;
    if (EventBuffer.rear == EventBuffer.front)
    {
        // 0 and 1 mean key0 and key1, 2 means no event
        return 0xff;
    }
    temp = EventBuffer.btnEventBuf[EventBuffer.front];
    EventBuffer.front = (EventBuffer.front + 1) % EventBuffer.bufsize;
    return temp;
}
