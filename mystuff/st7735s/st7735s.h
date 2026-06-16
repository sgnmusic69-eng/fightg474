#ifndef __ST7735S_H__
#define __ST7735S_H__

#include "main.h"
#include "gpio.h"
#include "spi.h"

#define RST_H HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET)
#define RST_L HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET)

#define DC_H HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET)
#define DC_L HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET)

#define CS_H HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET)
#define CS_L HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET)

#define RED    0xf800
#define GREEN  0x07e0
#define BLUE   0x001f
#define WHITE  0xffff
#define BLACK  0x0000
#define YELLOW 0xFFE0

void lcd_init(void);
void lcd_reset(void);
void lcd_write_address(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end);
void lcd_set_color(uint16_t color);
void lcd_set_area_color(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, uint16_t color);
void lcd_set_point_color(uint8_t x_point, uint8_t y_point, uint16_t color);
void showimage(uint8_t x, uint8_t y, uint8_t x2, uint8_t y2, const unsigned char *p);

void lcd_write_data(uint8_t data);
void lcd_write_command(uint8_t data);
void lcd_write_data_u16(uint16_t data);

void lcd_display_char_16x16(uint8_t x, uint8_t y, char c, uint16_t color);
void lcd_display_char_12x12(uint8_t x, uint8_t y, char c, uint16_t color);
void display_hz_16x16(uint8_t x, uint8_t y, const unsigned char *hz_data, uint16_t color);
void lcd_display_string_16x16(uint8_t x, uint8_t y, const char *str, uint16_t color);
void lcd_display_string_12x12(uint8_t x, uint8_t y, const char *str, uint16_t color);
void display_hz_string(uint8_t x, uint8_t y, uint8_t start_index, uint16_t color, uint8_t count);

void itoa_custom(int num, char* str, int base);
void lcd_display_number_12x12(uint8_t x, uint8_t y, int num, uint16_t color);
void lcd_display_number_16x16(uint8_t x, uint8_t y, int num, uint16_t color);

#endif
