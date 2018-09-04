/**
	*	@breif	delay some time , delay configuration , some us and some ms
	*	@param	nus,some millisecond
	*					ums,some microsecond
	*	@retval	None
	*	@note 	SysTick是一个24位倒计数定时器
	*	@mark		temp & 0x01 && (temp & (1<<16)):计数器向下计数，查询计数器当前值，当读取到的计数值temp不为0，
	*					即(temp & 0x01)并且计数标志位(1<<16，CTRL寄存器的第16bit)不为1时	(!(temp & (1<<16)))，继续循环。
	*/

#include "Delay.h"

static u16 fac_ms;
static u8 fac_us;

void Delay_Config(void)
{	
	/* 	choose clock source and setting correlation param	*/
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);				//选择AHB的8分频为滴答时钟的时钟源：72/8=9MHz
	fac_us=SystemCoreClock/8000000;
	fac_ms=(u16)fac_us*1000; 
}

void Delay_ms(u32 nms)
{
	u32 temp;
	/*	Config SysTick correlation param	*/
	//	SysTick_Config(ticks);				//以下的参数即是设置参数
	/* set reload register */	
	SysTick->LOAD  = nms*fac_ms;			//装初值
	/* Load the SysTick Counter Value */
  SysTick->VAL   = 0x00;								//清空计数器										
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;		//开始倒计时
	do{
		temp=SysTick->CTRL;

	}while(temp & 0x01 && !(temp & (1<<16)));		//等待时间到达
	SysTick->CTRL	&= ~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
	SysTick->VAL   = 0x00;													//清空计数器
}

void Delay_us(u32 nus)
{
	u32 temp;
	SysTick->LOAD =	nus*fac_us;					//计数初值，并装入初值寄存器
	SysTick->VAL	=	0x00;									//设置初值为0
	SysTick->CTRL|=	SysTick_CTRL_ENABLE_Msk;		//开始倒计数
	do{
		temp=SysTick->CTRL;
	}while(temp & 0x01 && !(temp & (1<<16)));		//等待计数时间到达
																							//(1<<16)为0x0001 0000
	SysTick->CTRL	&= ~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
	SysTick->VAL	=	0x00;														//清空计数器
}
