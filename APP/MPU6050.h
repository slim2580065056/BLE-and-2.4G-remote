/**
  ******************************************************************************
  * @file    MPU6050.c
  * @author  LiuSongHu_QQ2580065056
  * @version V1.0.0
  * @date    2016-12-25
  * @brief   This file provides MPU6050 operation gyro and accel firmware functions. 
  ******************************************************************************
  * @attention
  *	
  ******************************************************************************/
#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "stm32f10x.h"
#include "usart.h"
#include "Delay.h"
#include "I2C_Pro.h"
#include <math.h>
#include <stdio.h>

#define halfT					0.001f	//微分量
#define Kp  					100.0f	//比例增益
#define Ki   					0.002f	//积分增益

//sample rate divider	(Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV))
#define SMPRT_DIV			0x19		//陀螺仪采样率，典型值：0x07（125HZ）
//self-test register，在执行自检的时候陀螺仪范围应设置为250dps，加速度计范围应设置为+-8g
#define X_TEST				0X0D		//X轴陀螺仪和加速度自检
#define Y_TEST				0X0E		//Y轴陀螺仪和加速度自检
#define Z_TEST				0X0F		//Z轴陀螺仪和加速度自检
#define A_TEST				0X10		//加速度自检(上面的检4-2bit，这里检1-0bit)
//Configuration register
#define	CONFIG				0X1A
#define	GYRO_CONFIG		0X1B		//触发陀螺仪自检和设置陀螺仪量程
#define	ACCEL_CONFIG	0X1C		//触发加速度计自检和设置加速度的量程
//Accelerometer measurements(only_Read)
#define ACCEL_XOUT_H	0X3B
#define ACCEL_XOUT_L	0X3C
#define ACCEL_YOUT_H	0X3D
#define ACCEL_YOUT_L  0X3E
#define ACCEL_ZOUT_H	0X3F
#define ACCEL_ZOUT_L  0X40
//Temperature measurements(only_Read)
#define	TEMP_OUT_H		0X41		//Temperature in degrees C = (TEMP_OUT Register Value as a signed quantity)/340 + 36.53
#define TEMP_OUT_L    0X42
//Gyroscope measurements(only Read)
#define GYRO_XOUT_H		0X43
#define GYRO_XOUT_L		0X44
#define GYRO_YOUT_H   0X45
#define GYRO_YOUT_L   0X46
#define GYRO_ZOUT_H		0X47
#define GYRO_ZOUT_L  	0X48
//Signal Path Reset(only_write)
#define SIGNAL_PATH_RESET	0X68
//power mangment
#define PWR_MGMT_1		0X6B
#define PWR_MGMT_2		0X6C
//who am I
#define WHO_AM_I    	0X75
//hardware address
#define MPU6050_ADDR	0xD0		//MPU6050 hardware address is Addr|=(1<<0x68)

void MPU6050_Init(void);
void test_read(void);
void Data2Value(void);
void IMU_Update(void);
void ANO_IMU(void);
void ANO_Sensor(void);


float InvSqrt(float x);

#endif












