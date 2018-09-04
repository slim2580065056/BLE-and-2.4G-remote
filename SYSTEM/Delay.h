/**
	*	@breif	delay some time , delay configuration , some us and some ms
	*	@param	nus,some millisecond
	*					ums,some microsecond
	*	@retval	None
	*/

#ifndef __DELAY_H__
#define __DELAY_H__

#include "stm32f10x.h"

void Delay_Config(void);
void Delay_ms(u32);
void Delay_us(u32);

#endif
