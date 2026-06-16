#ifndef __AD9833_H__
#define __AD9833_H__

#include "main.h"
/*
 * AD9833 serial pins are unified in my_spi.h:
 * AD9833_SPI_SCK_* : PA7
 * AD9833_SPI_SDA_* : PA5
 * AD9833_SPI_CS1_* : PA6
 * AD9833_SPI_CS2_* : PA15
 */
/*控制字*/ //操纵哪几个
#define AD9833_ALL     0
#define AD9833_CH1     1
#define AD9833_CH2     2
/*寄存器*/
#define AD9833_Reg_Control  (0 << 14)
#define AD9833_Reg_Freq0		(1 << 14)
#define AD9833_Reg_Freq1		(2 << 14)

/*控制位*/
#define AD9833_Reg_Control_RESET (1 << 8)
#define AD9833_Reg_Control_B28   (1 << 13)

//
//#define AD9833_
/*波形类型*/
#define AD9833_Sinusoid     ((0 << 1) | (0 << 5) | (0 << 3))
#define AD9833_Triangle     ((0 << 5) | (1 << 1) | (0 << 3))



//#define AD9833_B28					(1 << 13)
#define AD9833_H1B					(1 << 12)
#define AD9833_fSel0				(0 << 11)
#define AD9833_fSel1				(1 << 11)
#define AD9833_pSel0				(0 << 10)
#define AD9833_pSel1				(1 << 10)
#define AD9833_Pin_SW				(1 << 9)
#define AD9833_Reset				(1 << 8)
#define AD9833_Sleep1				(1 << 7)
#define AD9833_Sleep2				(1 << 6)
#define AD9833_Opbiten      (1 << 5)
#define AD9833_Sign_Pib     (1 << 4)
#define AD9833_Div2         (1 << 3)
#define AD9833_Mode         (1 << 1)

#define AD9833_Out_Msb      ((1 << 5) | (0 << 1) | (1 << 3))
#define AD9833_Out_Msb2     ((1 << 5) | (0 << 1) | (0 << 3))

#define AD9833_MCLK_HZ      12300000UL   //主时钟

void AD9833_Init(void);
void SPI_Write_16bit(uint16_t data);//wasted
void AD9833_Write_Reg(uint16_t ch, uint16_t reg_data);

//void AD9833_Set_Register(uint16_t ch, uint16_t reg);
void AD9833_Set_Frequency(uint16_t ch, uint16_t Type, uint32_t frequency_hz);
void AD9833_Output_Sine(uint16_t ch, uint32_t frequency_hz);

#endif
