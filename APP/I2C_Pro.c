/**
  ******************************************************************************
  * @file    I2C_Pro.c
  * @author  LiuSongHu_QQ2580065056
  * @version V2.1.0
  * @date    2016-11-4
  * @brief   This file provides I2C time series functions. 
  ******************************************************************************
  * @attention	I2C1_SCL->PB8			I2C1_SDA->PB9
	*							输出模式：GPIO_Mode_AF_OD
  *	@remark	 		使用软件模拟的方式,传送的时候从高位开始传送
	*							接收完最后一个数据之后，将数据线设置成输出模式
  ******************************************************************************
	*/

#include "I2C_Pro.h"

/**
	*	@brief 	config GPIO
	*	@param 	None
	*	@retval	None
	*/
void I2C_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;
	
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	SCL_H;		//释放总线
	SDA_H;
}

void SDA_IN(void)  		//数据线为输入模式
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
	  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;		//浮空输入模式，被从机数据线SDA所影响，接收数据
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}
void SDA_OUT(void) 		//数据线为输出模式
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
	  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;				//开漏输出模式，因为I2C数据线接有上拉电阻，所以不用PP
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

/**
	*	@brief 	i2c start
	*	@param	None
	*	@retval	None
	*/
void I2C_Start(void)
{
	SDA_OUT();
	SDA_H;
	SCL_H;	
	Delay_us(1);
	SDA_L;
	Delay_us(1);
	SCL_L;		//允许数据变化，接收器件地址
}

/**
	*	@brief 	i2c stop
	*	@param 	None
	*	@retval	None
	*/
void I2C_Stop(void)
{
	SDA_OUT();
	SCL_H;
	SDA_L;
	Delay_us(1);
	SDA_H;
	Delay_us(1);
}

/**
	*	@brief 	i2c_master_ack(FlagStatus)
	*	@param	RESET->noack	/		SET->ack
	*	@retval	None
	*/
void I2C_Master_Ack(FlagStatus k)
{
	SDA_OUT();
	if(k)
	{
		SDA_L;
		SCL_H;
	}
	else
	{
		SDA_H;
		SCL_H;
	}
	Delay_us(1);
	SCL_L;
	SDA_L;
}

/**
	*	@brief 	i2c_check_ack
	*	@param	None
	*	@retval	ErrorStatus
	*/
ErrorStatus I2C_Check_Ack(void)
{
	SDA_IN();
	SCL_H;
	Delay_us(1);		//拉高总线5us
	if(GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_9)==RESET)		//PB9是否为低电平，应答
	{
		SCL_L;		//允许数据变化，接收后面的数据
		Delay_us(1);
		return SUCCESS;
	}
	else 		//非应答
	{
		SDA_L;
		SCL_L;		//允许数据变化，接收后面的数据
		Delay_us(1);
		return	ERROR;
	}
}

/**
	*	@brief 	i2c_send one byte
	*	@param	data
	*	@retval	None
	*	@remark	MSB to LSB
	*/
void I2C_SendByte(u8 data)
{
	u8 i;
	SDA_OUT();
	SDA_H;	//禁止数据变化
	for(i=0;i<8;i++)
	{
		SCL_L;		//允许数据变化
		((data&0x80)==0x80)?SDA_H:SDA_L;		//如果最高位为1则高电平
		Delay_us(1);
		SCL_H;		//禁止数据变化		
		Delay_us(1);
		data<<=1;
	}
	SCL_L;			//允许数据变化，判断应答位
	Delay_us(1);
}

/**
	*	@brief	read byte
	*	@param	None
	*	@retval	data
	*/
u8 I2C_ReadByte(void)
{
	u8 i,data;
	SDA_IN();	
	for(i=0;i<8;i++)
	{
		SCL_L;		//允许数据变化
		Delay_us(1);
		SCL_H;		//禁止数据变化
		data<<=1;
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9))
			data+=1;
		Delay_us(1);	
	}
	SCL_L;		//允许数据变化，判断应答
	Delay_us(1);
	return data;
}

/**
	*	@brief 	send the data register
	*	@param	save data register and Pointer to the data
	*	@retval	None
	*/
void I2C_Send_Data(u8 Addr,u8 data)
{
	I2C_Start();								//起始信号
	I2C_SendByte(hardware_Addr | 0);			//发送器件地址，写->0
	I2C_Check_Ack();						//检测应答
	I2C_SendByte(Addr);					//写入首地址
	I2C_Check_Ack();						//检测应答
	I2C_SendByte(data);						//写入数据
	I2C_Check_Ack();						//检测应答
	I2C_Stop();									//停止信号
}

/**
	*	@brief 	read the data from register
	*	@param	register address and should read how much byte
	*	@retval Pointer to the data of address
	*/
u8 I2C_Read_Data(u8 Addr)
{
	char p;
	I2C_Start();					//起始信号
	I2C_SendByte(hardware_Addr | 0);	//器件地址，写->0
	I2C_Check_Ack();			//应答
	I2C_SendByte(Addr);		//写入首地址
	I2C_Check_Ack();			//检测应答
	I2C_Start();					//起始信号
	I2C_SendByte(hardware_Addr | 1);	//器件地址，读->1
	I2C_Check_Ack();			//检测应答
	p=I2C_ReadByte();			//读出数据
	I2C_Master_Ack(RESET);	//主动非应答
	I2C_Stop();						//停止信号
	return p;
}

/**
	*	@brief 	用来读写XYZ轴的数字量，代替short I2C_Read_16(u8 REG_Address)，连续读写提高速度
	*	@param	register address
	*	@retval short data
	*/
short I2C_Read_16(u8 Addr)
{
	u16 p;
	I2C_Start();					//起始信号
	I2C_SendByte(hardware_Addr | 0);	//器件地址，写->0
	I2C_Check_Ack();			//应答
	I2C_SendByte(Addr);		//写入首地址
	I2C_Check_Ack();			//检测应答
	I2C_Start();					//起始信号
	I2C_SendByte(hardware_Addr | 1);	//器件地址，读->1
	I2C_Check_Ack();			//检测应答
	p=(I2C_ReadByte()&0x00ff)<<8;			//读出数据
	I2C_Master_Ack(SET);	//主动应答
	p=p | I2C_ReadByte();	//接收6bit
	I2C_Master_Ack(RESET);//主动非应答
	I2C_Stop();						//停止信号
	return p;
}
