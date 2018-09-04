/**
	*	@brief	config common periph_driver:example GPIO 
	*	@param	None
	*	@retval	None
	*	@periph 
	*******************************************************************************************
	*	@function		void RCC_Config(void);
	*							void NVIC_Config(void);
	*******************************************************************************************
	*/
#include "sys.h"
#include "usart.h"

//===============================================================================================================
void RCC_Config(void)
{
	//Error status variables 定义错误状态变量
	ErrorStatus HSEStartUpStatus;
	
	RCC_DeInit();									//将RCC寄存器重新设置为默认值
	
	RCC_HSEConfig(RCC_HSE_ON);		//打开外部高速晶振
//	while(RCC_WaitForHSEStartUp()==RESET);
	HSEStartUpStatus=RCC_WaitForHSEStartUp();
	if(HSEStartUpStatus==SUCCESS)
	{
		RCC_HCLKConfig(RCC_SYSCLK_Div1);	//HCLK为SYSCLK时钟
		RCC_PCLK1Config(RCC_SYSCLK_Div1);	//APB1为SYSCLK时钟2分频,这里设置为2时，SysTick出错（错误理解）
																			//设置AHB低速时钟为SYSCLK的一分频，即PCLK1最大支持36MHz（硬件）（正确解释）
		RCC_PCLK2Config(RCC_SYSCLK_Div1);	//APB2为SYSCLK时钟1分频（错误理解）
																			//设置AHB高速时钟为SYSCLK的一分频，即PCLK2（正确解释）
		/*	时钟树中，APB1可到：	1.PCLK1最大36MHz至APB1外设；
															2.TIMXCLK定时器2——7（如果APB1预分频系数=1，则频率不变，否则频率X2）	*/
		
		/*	set PLL clock ,PLLCLK=HSE(8MHz)*9=72MHz;	*/
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);
		RCC_PLLCmd(ENABLE);												//使能PLL时钟
		
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==RESET);		//等待PLL准备就绪
		
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);					//设置PLL为系统时钟源
		
		/* Judge PLLCLK is SYSCLK or not	*/ 
		while(RCC_GetSYSCLKSource()!=0x08);				//0x00:HSI作为系统时钟
																						//0x04:HSE作为系统时钟
																						//0x08:PLL作为系统时钟
	}
}
//===============================================================================================================

//===============================================================================================================
void NVIC_Config(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_Group);					//设置中断优先级分组
	
	NVIC_InitStructure.NVIC_IRQChannel=NVIC_Channel; 							//中断通道
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=NVIC_PreemptionPriority;  //抢占优先级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=NVIC_SubPriority;         //子优先级
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;  
	
	NVIC_Init(&NVIC_InitStructure);
}
