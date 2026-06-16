#include "st7735s.h"
#include "font.h"

static void lcd_spi_write(const uint8_t *data, uint16_t len)
{
	HAL_SPI_Transmit(&hspi3, (uint8_t *)data, len, HAL_MAX_DELAY);
}

void lcd_init()
{
	lcd_reset();//Reset before LCD Init.
		
	//LCD Init For 1.44Inch LCD Panel with ST7735R.
	lcd_write_command(0x11);//Sleep exit 
	HAL_Delay(120);
		
	//ST7735R Frame Rate	 4
	lcd_write_command(0xB1); 
	lcd_write_data(0x01); 
	lcd_write_data(0x2C); 
	lcd_write_data(0x2D); 
									
	lcd_write_command(0xB2); 
	lcd_write_data(0x01); 
	lcd_write_data(0x2C); 
	lcd_write_data(0x2D); 

	lcd_write_command(0xB3); 
	lcd_write_data(0x01); 
	lcd_write_data(0x2C); 
	lcd_write_data(0x2D); 
	lcd_write_data(0x01); 
	lcd_write_data(0x2C); 
	lcd_write_data(0x2D); 
	
	lcd_write_command(0xB4); //Column inversion 
	lcd_write_data(0x07); 
	
	//ST7735R Power Sequence
	lcd_write_command(0xC0); 
	lcd_write_data(0xA2); 
	lcd_write_data(0x02); 
	lcd_write_data(0x84); 
	lcd_write_command(0xC1); 
	lcd_write_data(0xC5); 

	lcd_write_command(0xC2); 
	lcd_write_data(0x0A); 
	lcd_write_data(0x00); 

	lcd_write_command(0xC3); 
	lcd_write_data(0x8A); 
	lcd_write_data(0x2A); 
	lcd_write_command(0xC4); 
	lcd_write_data(0x8A); 
	lcd_write_data(0xEE); 
	
	lcd_write_command(0xC5); //VCOM 
	lcd_write_data(0x0E); 
	
	lcd_write_command(0x36); //MX, MY, RGB mode 
//	lcd_write_data(0xA0); //竖屏C8 横屏08 A8
	lcd_write_data(0xC0); //竖屏C8 横屏08 A8 
	
	//ST7735R Gamma Sequence
	lcd_write_command(0xe0); 
	lcd_write_data(0x0f); 
	lcd_write_data(0x1a); 
	lcd_write_data(0x0f); 
	lcd_write_data(0x18); 
	lcd_write_data(0x2f); 
	lcd_write_data(0x28); 
	lcd_write_data(0x20); 
	lcd_write_data(0x22); 
	lcd_write_data(0x1f); 
	lcd_write_data(0x1b); 
	lcd_write_data(0x23); 
	lcd_write_data(0x37); 
	lcd_write_data(0x00); 	
	lcd_write_data(0x07); 
	lcd_write_data(0x02); 
	lcd_write_data(0x10); 

	lcd_write_command(0xe1); 
	lcd_write_data(0x0f); 
	lcd_write_data(0x1b); 
	lcd_write_data(0x0f); 
	lcd_write_data(0x17); 
	lcd_write_data(0x33); 
	lcd_write_data(0x2c); 
	lcd_write_data(0x29); 
	lcd_write_data(0x2e); 
	lcd_write_data(0x30); 
	lcd_write_data(0x30); 
	lcd_write_data(0x39); 
	lcd_write_data(0x3f); 
	lcd_write_data(0x00); 
	lcd_write_data(0x07); 
	lcd_write_data(0x03); 
	lcd_write_data(0x10);  
	
	lcd_write_command(0x2a);
	lcd_write_data(0x00);
	lcd_write_data(0x00+2);
	lcd_write_data(0x00);
	lcd_write_data(0x80+2);

	lcd_write_command(0x2b);
	lcd_write_data(0x00);
	lcd_write_data(0x00+3);
	lcd_write_data(0x00);
	lcd_write_data(0x80+3);
	
	lcd_write_command(0xF0); //Enable test command  
	lcd_write_data(0x01); 
	lcd_write_command(0xF6); //Disable ram power save mode 
	lcd_write_data(0x00); 
	
	lcd_write_command(0x3A); //65k mode 
	lcd_write_data(0x05); 
	
	
	lcd_write_command(0x29);//Display on
}


void lcd_reset()
{
	RST_L;//RST引脚拉低
	HAL_Delay(1);
	RST_H;//RST引脚拉高
	HAL_Delay(120);
}

//写入屏幕地址函数
void lcd_write_address(uint8_t x_start,uint8_t y_start,uint8_t x_end,uint8_t y_end)
{
	lcd_write_command(0x2a);
	lcd_write_data(x_start >> 8);
	lcd_write_data(x_start);
	lcd_write_data(x_end >> 8);
	lcd_write_data(x_end);
	
	lcd_write_command(0x2b);
	lcd_write_data(y_start >> 8);
	lcd_write_data(y_start);
	lcd_write_data(y_end >> 8);
	lcd_write_data(y_end);	
	
	lcd_write_command(0x2c);
}

////全屏颜色填充
//void lcd_set_color(uint16_t color)
//{
//	lcd_write_address(0,0,159,127);//像素160*128
//	for(int i = 0; i < 160; i++)
//	{
//		for(int j = 0; j < 128; j++)
//		{
//			lcd_write_data_u16(color);
//		}
//	}
//} 
//全屏颜色填充
void lcd_set_color(uint16_t color)
{
	lcd_write_address(0,0,127,159);//像素160*128
	for(int i = 0; i < 128; i++)
	{
		for(int j = 0; j < 160; j++)
		{
			lcd_write_data_u16(color);
		}
	}
} 
//区域颜色填充
void lcd_set_area_color(uint8_t x_start,uint8_t y_start,uint8_t x_end,uint8_t y_end,uint16_t color)
{
	lcd_write_address(x_start,y_start,x_end,y_end);
	//计算填充区域的长度和宽度，终点坐标减起点坐标+1
	uint8_t x_len = x_end - x_start + 1;//计算x坐标的长度
	uint8_t y_len = y_end - y_start + 1;//计算y坐标的长度
	for(int i = 0; i < x_len; i++)
	{
		for(int j = 0; j < y_len; j++)
		{
			lcd_write_data_u16(color);
		}
	}
}

//描点函数
void lcd_set_point_color(uint8_t x_point,uint8_t y_point,uint16_t color)
{
	lcd_write_address(x_point,y_point,x_point,y_point);
	lcd_write_data_u16(color);
}



//写数据: DC = 0
void lcd_write_data(uint8_t data)
{
	CS_L;
	DC_H;
	lcd_spi_write(&data, 1);
	CS_H;
}

//写操作: DC = 1
void lcd_write_command(uint8_t data)
{
	CS_L;
	DC_L;
	lcd_spi_write(&data, 1);
	CS_H;
}

//写十六位数据
void lcd_write_data_u16(uint16_t data)
{
	lcd_write_data(data >> 8);	
	lcd_write_data(data & 0xff);	
}

// 显示一个 12x12 的字符
void lcd_display_char_12x12(uint8_t x, uint8_t y, char c, uint16_t color)
{
    uint8_t i, j;
    const unsigned char *char_data = asc2_1206[c - 32];
    lcd_write_address(x, y, x + 11, y + 11);
    for (i = 0; i < 12; i++) {
        for (j = 0; j < 12; j++) {
            if (char_data[i] & (0x80 >> j)) {
                lcd_write_data_u16(color);
            } else {
                lcd_write_data_u16(BLACK); // 不显示的部分设置为黑色作为背景色
            }
        }
    }
}

// 显示一个 16x16 的字符
void lcd_display_char_16x16(uint8_t x, uint8_t y, char c, uint16_t color)
{
    uint8_t i, j;
    const unsigned char *char_data = asc2_1608[c - 32];
    lcd_write_address(x, y, x + 15, y + 15);
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 16; j++) {
            if (char_data[i] & (0x80 >> j)) {
                lcd_write_data_u16(color);
            } else {
                lcd_write_data_u16(BLACK); // 不显示的部分设置为黑色作为背景色
            }
        }
    }
}


// 显示字符串（使用 12x12 字符）
void lcd_display_string_12x12(uint8_t x, uint8_t y, const char *str, uint16_t color)
{
    uint8_t i = 0;
    while (str[i]!= '\0') {
        lcd_display_char_12x12(x, y, str[i], color);
        x += 6; // 下一个字符的起始位置
        i++;
    }
}

// 显示字符串（使用 16x16 字符）
void lcd_display_string_16x16(uint8_t x, uint8_t y, const char *str, uint16_t color)
{
    uint8_t i = 0;
    while (str[i]!= '\0') {
        lcd_display_char_16x16(x, y, str[i], color);
        x += 8; // 下一个字符的起始位置
        i++;
    }
}

// 将数字转换为字符串
void itoa_custom(int num, char* str, int base)
{
    int i = 0;
    int is_negative = 0;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    if (num < 0 && base == 10) {
        is_negative = 1;
        num = -num;
    }

    while (num!= 0) {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem - 10) + 'A' : rem + '0';
        num = num / base;
    }

    if (is_negative) {
        str[i++] = '-';
    }

    str[i] = '\0';

    // 反转字符串
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}


// 显示数字（使用 12x12 字符）
void lcd_display_number_12x12(uint8_t x, uint8_t y, int num, uint16_t color)
{
    char num_str[20];
    itoa_custom(num, num_str, 10);
    lcd_display_string_12x12(x, y, num_str, color);
}


// 显示数字（使用 16x16 字符）
void lcd_display_number_16x16(uint8_t x, uint8_t y, int num, uint16_t color)
{
    char num_str[20];
    itoa_custom(num, num_str, 10);
    lcd_display_string_16x16(x, y, num_str, color);
}
// 显示图片  C语言数组/水平扫描/16位真彩/不包含头数据
void showimage(uint8_t x,uint8_t y,uint8_t x2,uint8_t y2,const unsigned char *p) 
{
    unsigned char picH,picL;
    lcd_set_color(0XFFFF); //清屏  

    // 设置显示区域为x2*y2 图像在 160*128 屏幕的正中间
    lcd_write_address(x, y, x+x2-1, y+y2-1);
    
	for(uint16_t i=0;i<x2*y2;i++)
	 {	
			picL=*(p+i*2);	//数据低位在前
			picH=*(p+i*2+1);				
			lcd_write_data_u16(picH<<8|picL);  						
	 }
}

//PC_LCD2002  阴码，逐行式扫码 C51格式
// 显示单个 16x16 汉字
void display_hz_16x16(uint8_t x, uint8_t y, const unsigned char *hz_data, uint16_t color)
{
    uint8_t i, j;
    lcd_write_address(x, y, x + 15, y + 15); // 设置显示区域，16x16像素区域
    for (i = 0; i < 16; i++) {
        // 每一行由两个字节组成，分别表示该行的8个像素点
        uint8_t byte1 = hz_data[i * 2];     // 当前行的第一个字节
        uint8_t byte2 = hz_data[i * 2 + 1]; // 当前行的第二个字节
        
        // 遍历当前行的16个像素点（由两个字节表示）
        for (j = 0; j < 8; j++) {
            if (byte1 & (0x80 >> j)) {  // 判断第一个字节的当前像素点
                lcd_write_data_u16(color); // 显示点
            } else {
                lcd_write_data_u16(0x0000); // 背景色为黑色
            }
        }
        
        for (j = 0; j < 8; j++) {
            if (byte2 & (0x80 >> j)) {  // 判断第二个字节的当前像素点
                lcd_write_data_u16(color); // 显示点
            } else {
                lcd_write_data_u16(0x0000); // 背景色为黑色
            }
        }
    }
}


// 显示汉字字符串
void display_hz_string(uint8_t x, uint8_t y, uint8_t start_index, uint16_t color, uint8_t count)
{
    uint8_t i;
    for (i = 0; i < count; i++) {
        // 确保索引在有效范围内
        if (start_index + i < sizeof(chinese_font) / sizeof(chinese_font[0])) {
            // 从指定位置读取汉字
            const unsigned char *hz_data = chinese_font[start_index + i];
            display_hz_16x16(x, y, hz_data, color);
            x += 16; // 移动到下一个汉字的位置
        }
    }
}











