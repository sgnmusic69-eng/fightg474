#ifdef __cplusplus
#include <cstring>
#endif

#include <string.h>

#include "gui.h"
#include "oled.h"
#include "oledfont.h"
#include "string.h"


extern uint8_t F6x8[][8];
extern uint8_t F8X16[];

extern uint8_t Index16[];
extern typFNT_GB16 cfont16[];
extern uint8_t Index24[];
extern typFNT_GB24 cfont24[];
extern uint8_t Index32[];
extern typFNT_GB32 cfont32[];

static uint32_t myQpow(uint8_t m, uint8_t n)
{
    uint32_t result = 1, y = m;
    while (n)
    {
        if (n & 1)
            result = result * y;
        y *= y;
        n >>= 1;
    }
    return result;
}

static void Myswap(uint8_t *a, uint8_t *b)
{
    uint16_t tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

void GUI_DrawPoint(uint8_t x, uint8_t y, uint8_t color)
{
    OLED_Set_Pixel(x, y, color);
}

void GUI_Fill(uint8_t sx, uint8_t sy, uint8_t ex, uint8_t ey, uint8_t color)
{
    uint8_t i, j;
    uint8_t width = ex - sx + 1;
    uint8_t height = ey - sy + 1;
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            uint8_t pixel_x = sx + j;
            uint8_t pixel_y = sy + i;
            OLED_Set_Pixel(pixel_x, pixel_y, color);
        }
    }
}

void GUI_ReverseFill(uint8_t sx, uint8_t sy, uint8_t ex, uint8_t ey)
{
    uint8_t i, j;
    uint8_t width = ex - sx + 1;
    uint8_t height = ey - sy + 1;
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            uint8_t pixel_x = sx + j;
            uint8_t pixel_y = sy + i;
            OLED_Reverse_Pixel(pixel_x, pixel_y);
        }
    }
}

void GUI_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;

    delta_x = x2 - x1;
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    if (delta_x > 0)
        incx = 1;
    else if (delta_x == 0)
        incx = 0;
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }
    if (delta_y > 0)
        incy = 1;
    else if (delta_y == 0)
        incy = 0;
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    if (delta_x > delta_y)
        distance = delta_x;
    else
        distance = delta_y;
    for (t = 0; t <= distance + 1; t++)
    {
        OLED_Set_Pixel(uRow, uCol, color);
        xerr += delta_x;
        yerr += delta_y;
        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

void GUI_DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,
                       uint8_t color)
{
    GUI_DrawLine(x1, y1, x2, y1, color);
    GUI_DrawLine(x1, y1, x1, y2, color);
    GUI_DrawLine(x1, y2, x2, y2, color);
    GUI_DrawLine(x2, y1, x2, y2, color);
}

void GUI_DrawLine_Dashed(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,
                         uint8_t color, uint8_t dash_len, uint8_t gap_len)
{
    uint16_t t;
    int xerr = 0, yerr = 0;
    int delta_x = x2 - x1;
    int delta_y = y2 - y1;
    int incx, incy;
    int uRow = x1;
    int uCol = y1;
    int distance;
    uint8_t pattern_len = dash_len + gap_len;

    // Determine direction increments
    if (delta_x > 0)
        incx = 1;
    else if (delta_x == 0)
        incx = 0;
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)
        incy = 1;
    else if (delta_y == 0)
        incy = 0;
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    // Determine the number of steps needed
    distance = (delta_x > delta_y) ? delta_x : delta_y;

    for (t = 0; t <= (uint16_t)distance; t++)
    {
        // Draw pixel only within dash segment
        if ((t % pattern_len) < dash_len)
        {
            OLED_Set_Pixel(uRow, uCol, color);
        }

        // Increment error terms and step positions
        xerr += delta_x;
        yerr += delta_y;

        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

void GUI_DrawRectangle_Dashed(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color, uint8_t dash_len, uint8_t gap_len)
{
    GUI_DrawLine_Dashed(x1, y1, x2, y1, color, dash_len, gap_len);
    GUI_DrawLine_Dashed(x1, y1, x1, y2, color, dash_len, gap_len);
    GUI_DrawLine_Dashed(x1, y2, x2, y2, color, dash_len, gap_len);
    GUI_DrawLine_Dashed(x2, y1, x2, y2, color, dash_len, gap_len);
}

void GUI_ReverseRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
    uint8_t i, j;
    uint8_t Width, Height;
    if (x1 > 127)
    {
        return;
    }
    if (y1 > 63)
    {
        return;
    }
    if (x2 > 127)
    {
        Width = 127 - x1;
    }
    if (y2 > 63)
    {
        Height = 63 - y1;
    }

    for (j = y1; j < y1 + Height; j++) // ����ָ��ҳ
    {
        for (i = x1; i < x1 + Width; i++) // ����ָ����
        {
            OLED_Reverse_Pixel(i, j);
        }
    }
}

void GUI_FillRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,
                       uint8_t color)
{
    GUI_Fill(x1, y1, x2, y2, color);
}

void GUI_ReverseRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
    GUI_ReverseFill(x1, y1, x2, y2);
}

static void _draw_circle_8(uint8_t xc, uint8_t yc, uint8_t x, uint8_t y,
                           uint8_t color)
{
    OLED_Set_Pixel(xc + x, yc + y, color);
    OLED_Set_Pixel(xc - x, yc + y, color);
    OLED_Set_Pixel(xc + x, yc - y, color);
    OLED_Set_Pixel(xc - x, yc - y, color);
    OLED_Set_Pixel(xc + y, yc + x, color);
    OLED_Set_Pixel(xc - y, yc + x, color);
    OLED_Set_Pixel(xc + y, yc - x, color);
    OLED_Set_Pixel(xc - y, yc - x, color);
}

void GUI_DrawCircle(uint8_t xc, uint8_t yc, uint8_t color, uint8_t r)
{
    int x = 0, y = r, d;
    d = 3 - 2 * r;
    while (x <= y)
    {
        _draw_circle_8(xc, yc, x, y, color);
        if (d < 0)
        {
            d = d + 4 * x + 6;
        }
        else
        {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void GUI_FillCircle(uint8_t xc, uint8_t yc, uint8_t color, uint8_t r)
{
    int x = 0, y = r, yi, d;
    d = 3 - 2 * r;
    while (x <= y)
    {
        for (yi = x; yi <= y; yi++)
        {
            _draw_circle_8(xc, yc, x, yi, color);
        }
        if (d < 0)
        {
            d = d + 4 * x + 6;
        }
        else
        {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void GUI_DrawTriangel(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
                      uint8_t x2, uint8_t y2, uint8_t color)
{
    GUI_DrawLine(x0, y0, x1, y1, color);
    GUI_DrawLine(x1, y1, x2, y2, color);
    GUI_DrawLine(x2, y2, x0, y0, color);
}

static void _draw_h_line(uint8_t x0, uint8_t x1, uint8_t y, uint8_t color)
{
    uint8_t i = 0;
    for (i = x0; i <= x1; i++)
    {
        OLED_Set_Pixel(i, y, color);
    }
}

void GUI_FillTriangel(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
                      uint8_t x2, uint8_t y2, uint8_t color)
{
    uint8_t a, b, y, last;
    int dx01, dy01, dx02, dy02, dx12, dy12;
    long sa = 0;
    long sb = 0;
    if (y0 > y1)
    {
        Myswap(&y0, &y1);
        Myswap(&x0, &x1);
    }
    if (y1 > y2)
    {
        Myswap(&y2, &y1);
        Myswap(&x2, &x1);
    }
    if (y0 > y1)
    {
        Myswap(&y0, &y1);
        Myswap(&x0, &x1);
    }
    if (y0 == y2)
    {
        a = b = x0;
        if (x1 < a)
        {
            a = x1;
        }
        else if (x1 > b)
        {
            b = x1;
        }
        if (x2 < a)
        {
            a = x2;
        }
        else if (x2 > b)
        {
            b = x2;
        }
        _draw_h_line(a, b, y0, color);
        return;
    }
    dx01 = x1 - x0;
    dy01 = y1 - y0;
    dx02 = x2 - x0;
    dy02 = y2 - y0;
    dx12 = x2 - x1;
    dy12 = y2 - y1;

    if (y1 == y2)
    {
        last = y1;
    }
    else
    {
        last = y1 - 1;
    }
    for (y = y0; y <= last; y++)
    {
        a = x0 + sa / dy01;
        b = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        if (a > b)
        {
            Myswap(&a, &b);
        }
        _draw_h_line(a, b, y, color);
    }
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for (; y <= y2; y++)
    {
        a = x1 + sa / dy12;
        b = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        if (a > b)
        {
            Myswap(&a, &b);
        }
        _draw_h_line(a, b, y, color);
    }
}

void GUI_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size,
                  uint8_t mode)
{
    unsigned char c = 0, i = 0, tmp, j = 0;
    c = chr - ' ';
    if (x > WIDTH - 1)
    {
        x = 0;
        y = y + 2;
    }
    if (Char_Size == 16)
    {
        for (i = 0; i < 16; i++)
        {
            if (mode)
                tmp = F8X16[c * 16 + i];
            else
                tmp = ~(F8X16[c * 16 + i]);
            for (j = 0; j < 8; j++)
            {
                if (tmp & (0x80 >> j))
                    OLED_Set_Pixel(x + j, y + i, 1);
                else
                    OLED_Set_Pixel(x + j, y + i, 0);
            }
        }
    }
    else if (Char_Size == 8)
    {
        for (i = 0; i < 8; i++)
        {
            if (mode)
                tmp = F6x8[c][i];
            else
                tmp = ~(F6x8[c][i]);
            for (j = 0; j < 8; j++)
            {
                if (tmp & (0x80 >> j))
                    OLED_Set_Pixel(x + j, y + i, 1);
                else
                    OLED_Set_Pixel(x + j, y + i, 0);
            }
        }
    }
    else
        return;
}

void GUI_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t Size,
                 uint8_t mode)
{
    uint8_t t, temp;
    uint8_t enshow = 0, csize;
    if (Size == 16)
    {
        csize = Size / 2;
    }
    else if (Size == 8)
    {
        csize = Size / 2 + 2;
    }
    else
    {
        return;
    }
    for (t = 0; t < len; t++)
    {
        temp = (num / myQpow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                GUI_ShowChar(x + csize * t, y, ' ', Size, mode);
                continue;
            }
            else
                enshow = 1;
        }
        GUI_ShowChar(x + csize * t, y, temp + '0', Size, mode);
    }
}

void GUI_ShowString(uint8_t x, uint8_t y, uint8_t *chr, uint8_t Char_Size,
                    uint8_t mode)
{
    unsigned char j = 0, csize;
    if (Char_Size == 16)
    {
        csize = Char_Size / 2;
    }
    else if (Char_Size == 8)
    {
        csize = Char_Size / 2 + 2;
    }
    else
    {
        return;
    }
    while (chr[j] != '\0')
    {
        GUI_ShowChar(x, y, chr[j], Char_Size, mode);
        x += csize;
        if (x > 120)
        {
            x = 0;
            y += Char_Size;
        }
        j++;
    }
}

void GUI_ShowFont16(uint8_t x, uint8_t y, uint8_t *s, uint8_t mode)
{
    uint8_t i, j, k, tmp;
    uint16_t num;
    // to get size of buffer's size
    num = size_of_arr(16) >> 1;
    for (i = 0; i < num; i++)
    {
        if ((Index16[i << 1] == *s) && (Index16[(i << 1) + 1] == *(s + 1)))
        {
            for (j = 0; j < 32; j++)
            {
                if (mode)
                {
                    tmp = cfont16[i].Msk[j];
                }
                else
                {
                    tmp = ~(cfont16[i].Msk[j]);
                }
                for (k = 0; k < 8; k++)
                {
                    if (tmp & (0x80 >> k))
                    {
                        OLED_Set_Pixel(x + (j % 2) * 8 + k, y + j / 2, 1);
                    }
                    else
                    {
                        OLED_Set_Pixel(x + (j % 2) * 8 + k, y + j / 2, 0);
                    }
                }
            }
            break;
        }
    }
}

void GUI_ShowFont24(uint8_t x, uint8_t y, uint8_t *s, uint8_t mode)
{
    uint8_t i, j, k, tmp;
    uint16_t num;
    // to get size of buffer's size
    num = size_of_arr(24) >> 1;
    for (i = 0; i < num; i++)
    {
        if ((Index24[i << 1] == *s) && (Index24[(i << 1) + 1] == *(s + 1)))
        {
            for (j = 0; j < 72; j++)
            {
                if (mode)
                {
                    tmp = cfont24[i].Msk[j];
                }
                else
                {
                    tmp = ~(cfont24[i].Msk[j]);
                }
                for (k = 0; k < 8; k++)
                {
                    if (tmp & (0x80 >> k))
                    {
                        OLED_Set_Pixel(x + (j % 2) * 8 + k, y + j / 2, 1);
                    }
                    else
                    {
                        OLED_Set_Pixel(x + (j % 2) * 8 + k, y + j / 2, 0);
                    }
                }
            }
            break;
        }
    }
}

void GUI_ShowFont32(uint8_t x, uint8_t y, uint8_t *s, uint8_t mode)
{
    uint8_t i, j, k, tmp;
    uint16_t num;
    // to get size of buffer's size
    num = size_of_arr(32) >> 1;
    for (i = 0; i < num; i++)
    {
        if ((Index32[i << 1] == *s) && (Index32[(i << 1) + 1] == *(s + 1)))
        {
            for (j = 0; j < 128; j++)
            {
                if (mode)
                {
                    tmp = cfont32[i].Msk[j];
                }
                else
                {
                    tmp = ~(cfont32[i].Msk[j]);
                }
                for (k = 0; k < 8; k++)
                {
                    if (tmp & (0x80 >> k))
                    {
                        OLED_Set_Pixel(x + (j % 2) * 8 + k, y + j / 2, 1);
                    }
                    else
                    {
                        OLED_Set_Pixel(x + (j % 2) * 8 + k, y + j / 2, 0);
                    }
                }
            }
            break;
        }
    }
}

void GUI_ShowCHinese(uint8_t x, uint8_t y, uint8_t hsize, uint8_t *str,
                     uint8_t mode)
{
    while (*str != '\0')
    {
        if (hsize == 16)
        {
            GUI_ShowFont16(x, y, str, mode);
        }
        else if (hsize == 24)
        {
            GUI_ShowFont24(x, y, str, mode);
        }
        else if (hsize == 32)
        {
            GUI_ShowFont32(x, y, str, mode);
        }
        else
        {
            return;
        }
        x += hsize;
        if (x > WIDTH - hsize)
        {
            x = 0;
            y += hsize;
        }
        str += 2;
    }
}

void GUI_DrawBMP(uint8_t x, uint8_t y, uint8_t width, uint8_t height,
                 uint8_t BMP[], uint8_t mode)
{
    uint8_t i, j, k;
    uint8_t tmp;
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < (width + 7) / 8; j++)
        {
            if (mode)
            {
                tmp = BMP[i * ((width + 7) / 8) + j];
            }
            else
            {
                tmp = ~BMP[i * ((width + 7) / 8) + j];
            }
            for (k = 0; k < 8; k++)
            {
                if (tmp & (0x80 >> k))
                {
                    OLED_Set_Pixel(x + j * 8 + k, y + i, 1);
                }
                else
                {
                    OLED_Set_Pixel(x + j * 8 + k, y + i, 0);
                }
            }
        }
    }
}

/// @brief Show a string with both English and Chinese, only support 16 in size.
/// @param x
/// @param y
/// @param chr
/// @param mode
void GUI_ShowStringW(uint8_t x, uint8_t y, uint8_t *chr, uint8_t mode)
{
    uint8_t j = 0, csize = 8, Char_Size = 16;
    while (chr[j] != '\0')
    {
        if (chr[j] > '~')
        {
            GUI_ShowFont16(x, y, &chr[j], mode);
            x += Char_Size;
            j += 2;
        }
        else
        {
            GUI_ShowChar(x, y, chr[j], Char_Size, mode);
            x += csize;
            j++;
        }
        if (x > WIDTH - Char_Size)
        {
            x = 0;
            y += Char_Size;
        }
    }
}
