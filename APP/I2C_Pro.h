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
  *	@remark	 		使用软件模拟的方式
	*
  ******************************************************************************
	*/

#ifndef __I2C_PRO_H__
#define __I2C_PRO_H__

#include "stm32f10x.h"
#include "Delay.h"

#define SDA_H	GPIO_SetBits(GPIOB,GPIO_Pin_9)
#define SDA_L	GPIO_ResetBits(GPIOB,GPIO_Pin_9)
#define SCL_H	GPIO_SetBits(GPIOB,GPIO_Pin_8)
#define SCL_L	GPIO_ResetBits(GPIOB,GPIO_Pin_8)

#define	hardware_Addr 0xD0	//IIC写入时的地址字节数据,MPU6050address is Addr|=(1<<0x68)

//#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=0x0B<<7;}
//#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=0x08<<7;}

void I2C_GPIO_Config(void);
void SDA_OUT(void);
void SDA_IN(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Master_Ack(FlagStatus k);
ErrorStatus I2C_Check_Ack(void);
void I2C_SendByte(u8 data);
u8 I2C_ReadByte(void);

void I2C_Send_Data(u8 Addr,u8 data);
u8 I2C_Read_Data(u8 Addr);
short I2C_Read_16(u8 Addr);

#endif
