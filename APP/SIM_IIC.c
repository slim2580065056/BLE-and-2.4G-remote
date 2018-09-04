/************************************************
*		SCL->PB6
*		SDA->PB7
*		IIC在自定义函数的末尾处理SCL_L允许数据变化
************************************************/
#include "SIM_IIC.h"
#include "Delay.h"

extern u8 length;

void SIM_SCL_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;		//开漏型输出，IIC总线上有上拉电阻
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}
void SIM_SDA_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;		//开漏型输出，IIC总线上有上拉电阻
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}
void SIM_SDA_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;		//开漏型输出，IIC总线上有上拉电阻
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}
/*	IIC 总线初始化：置高总线	*/
void SIM_IIC_INIT(void)
{
	SIM_IIC_OUT;
	SIM_SCL_H;
	SIM_SDA_H;
}
/*  IIC 起始信号	*/
void SIM_IIC_START(void)
{
	SIM_IIC_OUT;
	SIM_SCL_H;
	SIM_SDA_H;
	Delay_us(5);
	SIM_SDA_L;
	Delay_us(5);
	SIM_SCL_L;			//data allow change,and received first bit
}
/*	IIC终止信号	*/
void SIM_IIC_STOP(void)
{
	SIM_IIC_OUT;
	SIM_SCL_H;
	SIM_SDA_L;
	Delay_us(5);
	SIM_SDA_H;
	Delay_us(5);	
}
/*	IIC应答测试	*/
AckStatus SIM_IIC_CHECK_ACK(void)
{
	SIM_IIC_IN;		//读取第九位状态
	SIM_SCL_L;		//允许读取数据，自定义函数末尾处理
	if(SIM_SDA_R==ACK)
	{
		SIM_SCL_H;		//拉高时钟5us
		Delay_us(1);
		SIM_SCL_L;		//data allow change,and receive next byte first bit
		return ACK;
	}
	else
	{
		SIM_IIC_STOP();
		return NACK;
	}
}
/*	IIC主动应答或主动非应答	*/
void SIM_IIC_MASTER_ACK(AckStatus flag)
{
	SIM_IIC_OUT;
//	SCL_L;		//允许读取数据，自定义函数末尾处理
	(flag==ACK)?SIM_SDA_L:SIM_SDA_H;		//flag为0，即应答
	SIM_SCL_H;			//数据线置高
	Delay_us(1);
	SIM_SCL_L;			//data allow change ,and receive next
}
/*	IIC发送一个字节数据，从MSB开始	*/
void SIM_SEND_BYTE(u8 byte)
{
	u8 i;
	SIM_IIC_OUT;
	SIM_SCL_L;		  //允许第一个数据变化
	/*	取byte中的最低位，如果为1则SDA为高，反之SDA为低	*/
	for(i=0;i<8;i++)
	{
		SIM_SCL_L;		//数据允许变化
		(byte&0x80)==0x80?SIM_SDA_H:SIM_SDA_L;		//判断MSB	
		Delay_us(1);
		SIM_SCL_H;		//数据不允许变化
		byte<<=1;	//byte左移一位		
	}
	SIM_SCL_L;			//数据允许变化，接收应答位
}
/*	IIC接收一个字节数据，从MSB开始	*/
u8 SIM_READ_BYTE(void)
{
	u8 i,byte=0;
	SIM_IIC_IN;
	SIM_SCL_H;
	for(i=0;i<8;i++)
	{
		byte|=(SIM_SDA_R&0x80);	//与最低位比较
		SIM_SCL_L;				//数据允许变化
		Delay_us(1);	//数据变化时间，防止出错
		byte<<=1;			//数据左移一位
		SIM_SCL_H;				//数据不允许变化
	}
	SIM_SCL_L;			//数据允许变化，接收应答
	return byte;
}
/*	发送一个指向P，长度为length的字符	*/
void SIM_SEND_DATA(u8 addr,u8 *P)
{
	u8 i;//,flag;
	i=0;
	SIM_IIC_START(); 									//START
	SIM_SEND_BYTE(Device_addr);				//DEVICE_ADDRESS & WRITE
	SIM_IIC_CHECK_ACK();					//ACK
	SIM_SEND_BYTE(addr);							//WORD_ADDRESS
	SIM_IIC_CHECK_ACK();					//ACK	//	while(length--)
	while(*P!=NULL)
	{
		i++;
		SIM_SEND_BYTE(*P++);							//DATA
		SIM_IIC_CHECK_ACK();				//ACK
	}	
	printf("is %d\r\n",i);
	SIM_IIC_STOP();										//STOP
}
/*	接收一个字符串，并返回该字符串地址	*/
//u8 *SIM_READ_DATA(u8 addr,u8 length)
//{
//	u8 *p,flag;
//	IIC_START();									//START
//	SEND_BYTE(Device_addr);				//DEVICE_ADDRESS & WRITE
//	flag=IIC_CHECK_ACK();					//ACK
//	SEND_BYTE(addr);							//WORD_ADDRESS n
//	flag=IIC_CHECK_ACK();					//ACK
//	IIC_START();
//	SEND_BYTE(Device_addr | 1);
//	flag=IIC_CHECK_ACK();
//	while(length--)			//用break跳出
//	{
//		*p++=READ_BYTE();						//DATA n
//		IIC_MASTER_ACK(length!=1?ACK:NACK);				//ACK
//	}
//	return p;
//}
