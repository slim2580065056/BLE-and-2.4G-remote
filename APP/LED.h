#ifndef __LED_H__
#define __LED_H__

#include "stm32f10x.h"
#include "sys.h"

#define LED1	PAout(4)
#define LED2  PAout(5)
#define LED3  PAout(6)

void LED_Config(void);
void LED1_Set(u16 rate);
void LED2_Set(u16 rate);
void LED3_Set(u16 rate);


#endif
