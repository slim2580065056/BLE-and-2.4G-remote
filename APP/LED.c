/*	@LED				A4  A5  A6			*/

#include "LED.h"

void LED_Config(void)
{
	RCC->APB2ENR|=1<<2;			//使能APB2_GPIOA时钟
	
	GPIOA->CRL&=0xF000FFFF;	//配置PIN4 PIN5 PIN6
	GPIOA->CRL|=0x03330000;	//配置为推挽输出
	
	GPIOA->ODR|=7<<4;				//0111 0000 = 0000 0111 << 4
}

/**
	*	@brief  Set LED rate
	*	@param  rate
	*	@retval None
	*/
void LED1_Set(u16 rate)
{
	static u16 i;
	if(0==rate)		LED1=0;		
	else if(1==rate) LED1=1;
	else if(++i==rate) i=0,LED1=!LED1;
}

void LED2_Set(u16 rate)
{
	static u16 i;
	if(0==rate)		LED2=0;		
	else if(1==rate)	LED2=1;
	else if(++i==rate) i=0,LED2=!LED2;
}

void LED3_Set(u16 rate)
{
	static u16 i;
	if(0==rate)		LED3=0;		
	else if(1==rate)	LED3=1;
	else if(++i==rate) i=0,LED3=!LED3;
}


