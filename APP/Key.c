//*	@Key				Key1->C15		Key2->C14		Key3->C13
//*	@Right_Lever  SW->B11		
//*	@Left_Lever		SW->A15		

#include "Key.h"

void Key_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,ENABLE);
	// Key1 / Key2 / Key3 -> IPU
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;	
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	// SW1 -> IPU
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPU;		//SW按键上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	// SW2 -> IPU
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPU;		//SW按键上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}

/**
	*	@brief	just judge key press down
	*	@param  None
	*	@retval Key_Value
	*/
u8 Key_Scan(void)
{
	u8 i,temp;
	for(i=0;i<5;i++)													//	for(i=1;i<6;i++)
	{																					//	{
		temp=KEY_VAL>>i;												//		temp=KEY_VAL<<i;
		if(0x1f!=KEY_VAL && 0==(temp & 0x01))		//		if(0x1f!=KEY_VAL && 0==(temp & 0x20))		//有键按下
			return i+1;						//返回键值			//			return i;	
	}																					//	}
	return 0;																	//	return 0;
}

/**
	*	@brief	获取键值
	*	@param  None
	*	@retval Key_Value
	*/
u8 Get_Key(void)
{
	static u8 n,status=0,last_val;
	u8 key_val;
	key_val=Key_Scan();									//键值赋值给key_val
	if(0!=key_val && key_val==last_val)	//如果在空闲模式下有键按下
	{
		if(0==status)	status=1;						//按键按下标识
		else 	n++;
	}
	if(1==status && key_val==0)					//按键按下标识使能且按键已经释放
	{
		status=0;													//按键释放标识
		if(n>2 && n<25)	//40ms ~ 500ms		//短按单击时间控制，根据控制周期时间控制次数
		{
			n=0;														//清除计数
			return last_val;								//返回键值
		}
		else if(n>=25)		//大于 500ms			//长按时间控制
		{
			n=0;														//清除计数
			return last_val+10;							//长按键值 = (短按键值+10)
		}
		else 															//按键时间过短被认为是按键抖动或误按状态
		{
			n=0;														//清除计数
			return 20;											//返回误按状态
		}
	}
	last_val=key_val;										//保存上一次的键值
	return 0;
}





