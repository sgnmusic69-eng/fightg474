/*

BSD 2-Clause License

Copyright (c) 2024, TcMcKrLiTb

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#ifndef __OLED_H__
#define __OLED_H__

/**
 * @file oled.h
 *
 * @brief base lib of 0.96inch 4-wire-SPI oled
 *
 */

#include "ti_msp_dl_config.h"

//--------------OLED parameters---------------------
#define PAGE_SIZE 8
#define XLevelL 0x00
#define XLevelH 0x10
#define YLevel 0xB0
#define Brightness 0xFF
#define WIDTH 128
#define HEIGHT 64

#define OLED_CMD 0  // write command
#define OLED_DATA 1 // write data

#define OLED_DC_Clr()  DL_GPIO_clearPins(GPIO_OLED_PORT, GPIO_OLED_PIN_DC_PIN)
#define OLED_DC_Set()  DL_GPIO_setPins(GPIO_OLED_PORT, GPIO_OLED_PIN_DC_PIN)

#define OLED_RST_Clr() DL_GPIO_clearPins(GPIO_OLED_PORT, GPIO_OLED_PIN_RES_PIN)
#define OLED_RST_Set() DL_GPIO_setPins(GPIO_OLED_PORT, GPIO_OLED_PIN_RES_PIN)

void OLED_WR_Byte(unsigned dat, unsigned cmd);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_Reset(void);
void OLED_Init_GPIO(void);
void OLED_Init(void);
void OLED_Set_Pixel(unsigned char x, unsigned char y, unsigned char color);
void OLED_Reverse_Pixel(uint8_t x, uint8_t y);
void OLED_Display(void);
void OLED_Clear(unsigned dat);
void OLED_transition(uint8_t x);

#endif
