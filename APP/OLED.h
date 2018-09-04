#ifndef __OLED_H__
#define __OLED_H__

#include "stm32f10x.h"
#include "SIM_IIC.h"

#define BRIGHT 				0x81
#define DISPLAY				0xAF
#define PAGE_ADDR			0xB0
#define COL_LOW4			0x0F
#define COL_HIGH4			0x1F
#define COL_ADDR			0x21
#define MEMORY_ADDR		0x20

void OLED_CMD(u8 cmd);
void OLED_DATA(u8 data);
void OLED_Init(void);
void Start_Pic(void);
void OLED_Clear(void);
void OLED_Area_Clear(void);
void OLED_Pos(u8 x,u8 y);
void OLED_PIC(const u8 *Pic);
void OLED_C6X8(u8 x,u8 y,u8 byte);
void OLED_S6X8(u8 x,u8 y,u8 *string);
void OLED_C8X16(u8 x,u8 y,u8 byte);
void OLED_S8X16(u8 x,u8 y,u8 *string);
u32 oled_pow(u8 m,u8 n);
void OLED_S6X8Num(u8 x,u8 y,u32 num,u8 len,u8 mode);
	
#endif


