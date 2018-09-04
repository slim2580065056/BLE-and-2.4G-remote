//	@Toggle_Switch	1->B3			2->B4  		3->B5
#include "Toggle_Switch.h"

void Switch_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	/*!< JTAG-DP Disabled and SW-DP Enabled */
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//PB3和PB4默认为Jlink端口，使用IO口需重映射
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;			//下拉输入，默认接地，接电源VCC时为高电平
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

u8 Switch_Code(void)
{
	static u8 code=0;		//初始化为0
	code=((PBin(5)&0101)<<2 | (PBin(4)&0x01)<<1 | PBin(3));
	return code;
}

