//*	@Beep				beep->A12

#include "Beep.h"

/**
	*	@brief  Configured Beep of GPIO
	*	@param  None
	*	@retval None
	*/
void Beep_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_12;
	
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	PAout(12)=0;
}

/**
	*	@brief  Set Beep rate and enable Ring
	*	@param  rate
	*	@retval None
	*/
void Beep_Set(u16 rate)
{
	static u16 i;
	if(0==rate)		PAout(12)=0;		//0Í£Ö¹Ïì
	else if(++i==rate) i=0,PAout(12)=!PAout(12);
}


