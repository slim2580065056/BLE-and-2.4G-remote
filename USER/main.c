/***************************@PinMap*****************************************												
*	@NRF24L01		CSN->B12	SCK->B13	MISO->B14		MOSI->B15		IRQ->A8		CE->A11
*	@MPU6050		SCL->B8		SDA->B9		INT->B10
*	@OLED				SCL->B6		SDA->B7
*	@Right_Lever  SW->B11		X->B1(ADC_IN9)		Y->B0(ADC_IN8)
*	@Left_Lever		SW->A12		X->A1(ADC_IN1)		Y->A0(ADC_IN0)
*	@Toggle_Switch	1->B3			2->B4  		3->B5
*	@Key				Key1->C15		Key2->C14		Key3->C13
*	@HC-05			Rx->A2		Tx->A3
*	@USART1			Tx->A9		Rx->A10
*	@Beep				beep->A12
*	@LED				A4  A5  A6
*	@Voltage		ADCx
*	@Control_Pro  5ms_Interrupt to control Program
***************************************************************************/

#include "stm32f10x.h"
#include "sys.h"

extern u16 AD_Value[4];
extern u8 Key_Val;
extern u16 MPU_Value[3];
extern Bit_Flag Flag;

u8 temp[]={0x5a};
//u8 ASCII2[]={" !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_'abcdefghijklmnopqrstuvwxyz"};

int main(void)
{
//	u8 i,temp_buf[15]={'L','S','H','@','$',};
	RCC_Config();										//系统时钟初始化
	Delay_Config();									//延时函数初始化
	USART1_Config(115200);					//串口1初始化，波特率115200bps
	BLE_Config(9600);								//蓝牙串口通信初始化		/////蓝牙暂时用扫描方式收发，升级之后改为中断方式，提高实时性
		USART_SendData(USART2,'1');		//测试蓝牙发送，发送成功，向上位机扣'1'
	LED_Config();										//LED配置，初始化灯亮
	Beep_Config();									//Beep蜂鸣器初始化
	Switch_Config();								//拨码开关初始化
	Key_Config();										//按键开关初始化
	I2C_GPIO_Config();							//先使能IIC的IO，提供MPU6050通信使用
	MPU6050_Init();									//MPU6050初始化	
	SPI2_NRF_CONFIG();							//配置SPI2，用于NRF24L01通信
	(0==NRF_Check())?LED1_Set(0):LED3_Set(0);
	Control_Lever_Config();					//摇杆SW开关及ADC配置
	OLED_Init();										//OLED初始化	
		Start_Pic();									//OLED开机动画
	TIM2_Config();									//TIM2时基配置
	printf("test\r\n");	
	while(1)
	{
		Show_Screen();
		if(Flag.LEV_BLE_Flag==1)			LED1_Set(2),LED2_Set(0),LED3_Set(0),LEV_BLE();
		else if(Flag.MPU_BLE_Flag==1)	LED1_Set(2),LED2_Set(2),LED3_Set(0),MPU_BLE();
		else if(Flag.LEV_NRF_Flag==1)	LED1_Set(0),LED2_Set(0),LEV_NRF();
		else if(Flag.MPU_NRF_Flag==1)	LED1_Set(0),LED2_Set(2),MPU_NRF();
		else if(Flag.NRF_BLE_Flag==1)	LED1_Set(2),LED2_Set(0),LED3_Set(2),NRF_BLE();
		else if(Flag.BLE_NRF_Flag==1)	LED1_Set(2),LED2_Set(0),BLE_NRF();
		if(Flag.SET_Flag==0 && Key_Val==1)	OLED_Clear(),OLED_Init();
		//add you code in here
	}
}





