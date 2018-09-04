//*	@Right_Lever  SW->B11		X->B1(ADC_IN9)		Y->B0(ADC_IN8)
//*	@Left_Lever		SW->A15		X->A1(ADC_IN1)		Y->A0(ADC_IN0)
//* @SW 在 Key.c 中设置

#include "Lever.h"

u16 AD_Value[4];		//用来存放ADC转换结果，也是DMA的目标地址
extern u8 ASCII[10];//={'0','1','2','3','4','5','6','7','8','9'};			//将数字转化为ASCII码
extern u8 AD_Flag[4];//={'a','b','c','d'};					//摇杆的特征码，用于识别收发的是哪个摇杆的AD值

void Control_Lever_Config(void)
{
	GPIO_InitTypeDef 	GPIO_InitStructure;
	ADC_InitTypeDef 	ADC_InitStructure;
	DMA_InitTypeDef   DMA_InitStructure;
	//=========================================================================================================
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);		//72/6=12,ADC最大转化时间不超过14MHz
	//=========================================================================================================
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | \
												 RCC_APB2Periph_AFIO | RCC_APB2Periph_ADC1,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	//X & Y ->ADC
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AIN;						//X/Y模拟输入
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0 |GPIO_Pin_1;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AIN;						//X/Y模拟输入
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0 |GPIO_Pin_1;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//ADC1_Config
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;  //独立工作模式
  ADC_InitStructure.ADC_ScanConvMode=ENABLE;       	//模数转换工作在扫描模式
  ADC_InitStructure.ADC_ContinuousConvMode=ENABLE; 	//模数转换在连续转换模式
  ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;    //不使能触发
  ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;                 //数据格式为右对齐
  ADC_InitStructure.ADC_NbrOfChannel=4;  						//4个通道转换
	ADC_Init(ADC1,&ADC_InitStructure);
	
	/*	ADC1 regular channel11 Config	*/
	/*		设置指定ADC的规则通道，设置他们的转化顺序和采样时间
				ADC1，ADC通道x,规则采样顺序值为y，采样时间为239.5；		*/
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,2,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_8,3,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_9,4,ADC_SampleTime_239Cycles5);
	
	//DMA1_Config
	DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)&ADC1->DR; 	//DMA外设ADC基地址
  DMA_InitStructure.DMA_MemoryBaseAddr=(u32)AD_Value;    		//DMA内存基地址
  DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;        	//内存作为数据传输的目的地
  DMA_InitStructure.DMA_BufferSize=4;         							//DMA通道DMA缓存数据大小
  DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable; 		//外设地址寄存器不变
  DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;   	//DMA内存地址寄存器增加
  DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord; //数据宽度16bit
  DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;					//数据宽度16bit
  DMA_InitStructure.DMA_Mode=DMA_Mode_Circular;         		//工作在循环缓存模式
  DMA_InitStructure.DMA_Priority=DMA_Priority_High;     		//DMA通道拥有高优先级
  DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;                //DMA通道x没有设置为内存到内存传输

	DMA_Init(DMA1_Channel1,&DMA_InitStructure);
	
	ADC_Cmd(ADC1, ENABLE);
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	//开启ADC的DMA支持
	ADC_DMACmd(ADC1,ENABLE);
	
	ADC_Cmd(ADC1, ENABLE);
	//复位ADC校准寄存器
	ADC_ResetCalibration(ADC1);		//复位制定ADC1的校准寄存器
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	//校准ADC
	ADC_StartCalibration(ADC1);		//开始校准ADC1
	while(ADC_GetCalibrationStatus(ADC1));
}

//void AD_Num(void)
//{
//	u8 i,temp[4];
//	for(i=0;i<4;i++)
//	{
//		temp[i]=AD_Value[i]/410;
//		printf("%6d      ",temp[i]);
//		/*	数据包加入特征码，用于从机识别接收的是哪个摇杆的AD值	*/
//		USART_SendData(USART2,(u8)ASCII[AD_Flag[i]]<<8 | (u8)ASCII[temp[i]]);
//		Delay_ms(10);				//每隔10ms发送一次，否则蓝牙收发异常
//	}
//	printf("\r\n");
//}


