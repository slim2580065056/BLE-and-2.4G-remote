/**
  ******************************************************************************
  * @file    MPU6050.c
  * @author  LiuSongHu_QQ2580065056
  * @version V1.0.0
  * @date    2016-12-25
  * @brief   This file provides MPU6050 operation gyro and accel firmware functions. 
  ******************************************************************************
  * @attention
	*	1.重力加速度归一化
	*	2.提取四元素的等效余弦矩阵中的重力分量
	*	3.向量叉积得出姿态误差
	*	4.对误差进行积分
	*	5.互补滤波，姿态误差补偿到角速度上，修正加速度的积分漂移
	*	6.一阶龙格库塔法更新四元素
	*	7.四元素归一化
	*	8.四元素转欧拉角
  */
	
#include "MPU6050.h"

typedef struct IMU_DATA
{
	float q0,q1,q2,q3;									//四元素	
	float ax,ay,az,gx,gy,gz;						//陀螺仪和加速度计的模拟量所对应的数字量
	float ex,ey,ez;											//滤波融合之后的三轴量
	float PITCH;				//仰俯角
	float YAW;					//偏航角
	float ROLL;					//横滚角

}IMU_param;
IMU_param IMU={1.0,0.0,0.0,0.0};	//初始化四元素;

u16 MPU_Value[3];

//extern float PITCH;				//仰俯角
//extern float YAW;					//偏航角
//extern float ROLL;					//横滚角

float exInt,eyInt,ezInt;

/***********************************************************************************************
*		函数：MPU6050芯片复位初始化程序
*		输入：无
*		输出：(串口读取)初始化成功 OR 初始化失败
*		说明：初始化电源选项，初始化采样率，初始化量程等
************************************************************************************************/
void MPU6050_Init(void)
{
	I2C_Send_Data(PWR_MGMT_1,0x80);						//复位电源管理，即复位所有寄存器
	Delay_ms(500);
	I2C_Send_Data(PWR_MGMT_1,0x00);						//停止休眠
	I2C_Send_Data(SMPRT_DIV,0x07);						//陀螺仪采样率，典型值0x07 -> 125HZ
	I2C_Send_Data(CONFIG,0x06);								//低通滤波设置,截止频率1K,带宽5K,延时19Ms
	I2C_Send_Data(SIGNAL_PATH_RESET,0x07);		//复位加速度计，复位陀螺仪，复位温度传感器
	I2C_Send_Data(GYRO_CONFIG,0x18);					//陀螺仪不自检，范围2000deg/s；(GYRO_CONFIG,0x10--1000deg/s)
	I2C_Send_Data(ACCEL_CONFIG,0x01);					//加速度不自检，范围+-4g；(00--+-2g)
//	printf("WHO_AM_I 0x%X  PWR_DIV 0x%X\r\n",I2C_Read_Data(WHO_AM_I),I2C_Read_Data(PWR_MGMT_1));//打印器件地址和复位寄存器值
	if(I2C_Read_Data(WHO_AM_I)==0x68)					//读出器件地址正确，则初始化成功
		printf("MPU6050 Initialized Complete \r\n");
	else
	{
		printf("MPU6050 Initialized Error \r\n");
		printf("driver id is %#x\r\n",I2C_Read_Data(WHO_AM_I));
	}
}
void test_read(void)
{
	IMU_Update();
//	printf("ax=%f  ay=%f  az=%f    ",IMU.ax,IMU.ay,IMU.az);
//	printf("gx=%f  gy=%f  gz=%f    \r\n",IMU.gx,IMU.gy,IMU.gz);
	printf("pitch->%.3f    roll->%.3f    yaw->%.3f \r\n",IMU.PITCH,IMU.ROLL,IMU.YAW);
}
/***********************************************************************************************
*		函数：MPU6050所读取的数字量转化为有用的模拟值
*		输入：无
*		输出：三轴的加速度模拟值和陀螺仪模拟值(作为全局变量，而不是返回值)
*		说明：将偏移量融合到数据中修正传感器的机械误差
************************************************************************************************/
void Data2Value(void)
{
		/*	加速度计量程+-4g，查表得其分度值（即灵敏度）2^13=8192LSB/g		*/
	IMU.gx=(float)I2C_Read_16(GYRO_XOUT_H)/8192;
	IMU.gy=(float)I2C_Read_16(GYRO_YOUT_H)/8192;
	IMU.gz=(float)I2C_Read_16(GYRO_ZOUT_H)/8192;
	/*	计算加速度和陀螺仪角度，注意单位	*/
	/*	陀螺仪量程+-2000deg/s，查表得其分度值（即灵敏度）=2^14=16.384LSB°/s		*/
	IMU.ax=(float)I2C_Read_16(ACCEL_XOUT_H)/16.384;
	IMU.ay=(float)I2C_Read_16(ACCEL_YOUT_H)/16.384;
	IMU.az=(float)I2C_Read_16(ACCEL_ZOUT_H)/16.384;
}

float InvSqrt(float x)
{
float xhalf = 0.5f*x;
int i = *(int*)&x;
i = 0x5f3759df - (i >> 1); // ????????
x = *(float*)&i;
x = x*(1.5f - xhalf*x*x); // ?????
return x;
}
/***********************************************************************************************
*		函数：MPU6050姿态更新融合解算
*		输入：三轴陀螺仪值和三轴加速度值，直接进行获取全局变量
*		输出：ROLL(横滚角)，PITCH(仰俯角)，YAW(偏航角)
*		说明：
************************************************************************************************/
void IMU_Update(void)
{
	float vx,vy,vz,ex,ey,ez;
	float norm;
	
	Data2Value();
	
	//归一化数据，方便处理
	norm=sqrt(IMU.ax*IMU.ax+IMU.ay*IMU.ay+IMU.az*IMU.az);		//sqrt(x^2+y^2+z^2)
	IMU.ax=IMU.ax/norm;
	IMU.ay=IMU.ay/norm;
	IMU.az=IMU.az/norm;
	
	vx=2*(IMU.q1*IMU.q3-IMU.q0*IMU.q2);
	vy=2*(IMU.q2*IMU.q3+IMU.q0*IMU.q1);
	vz=(IMU.q0*IMU.q0-IMU.q1*IMU.q1-IMU.q2*IMU.q2+IMU.q3*IMU.q3);
	
	ex=(IMU.ay*vz-IMU.az*vy);
	ey=(IMU.az*vx-IMU.ax*vz);
	ez=(IMU.ax*vy-IMU.ay*vx);
	
	exInt+=ex*Ki;
	eyInt+=ey*Ki;
	ezInt+=ez*Ki;
	
	IMU.gx+=Kp*ex+exInt;
	IMU.gy+=Kp*ey+eyInt;
	IMU.gz+=Kp*ez+ezInt;
	
	IMU.q0=IMU.q0+(-IMU.q1*IMU.gx-IMU.q2*IMU.gy-IMU.q3*IMU.gz)*halfT;
	IMU.q1=IMU.q1+(IMU.q0*IMU.gx+IMU.q2*IMU.gz-IMU.q3*IMU.gy)*halfT;
	IMU.q2=IMU.q2+(IMU.q0*IMU.gy-IMU.q1*IMU.gz+IMU.q3*IMU.gx)*halfT;
	IMU.q3=IMU.q3+(IMU.q0*IMU.gz+IMU.q1*IMU.gy-IMU.q2*IMU.gx)*halfT;
	
	//归一化四元素数据，方便处理
	norm=sqrt(IMU.q0*IMU.q0+IMU.q1*IMU.q1+IMU.q2*IMU.q2+IMU.q3*IMU.q3);
	IMU.q0=IMU.q0/norm;
	IMU.q1=IMU.q1/norm;
	IMU.q2=IMU.q2/norm;
	IMU.q3=IMU.q3/norm;

	/*	calculation  angle	,remark 180/Pi=57.293	*/
	IMU.YAW=atan2(2*IMU.q1*IMU.q2+IMU.q0*IMU.q3,-2*IMU.q2*IMU.q2-2*IMU.q3*IMU.q3+1)*57.297;
	IMU.PITCH=asin(-2*IMU.q1*IMU.q3+2*IMU.q0*IMU.q2)*57.297;
	IMU.ROLL=atan2(2*IMU.q2*IMU.q3+2*IMU.q0*IMU.q1,-2*IMU.q1*IMU.q1-2*IMU.q2*IMU.q2+1)*57.297;
	
	MPU_Value[0]=(u16)IMU.PITCH;	MPU_Value[1]=(u16)IMU.ROLL;	MPU_Value[2]=(u16)IMU.YAW;	
}
/***********************************************************************************************
*		函数：MPU6050姿态发送到匿名四轴上位机
*		说明：遵守匿名四轴上位机的协议
*			下位机发送自定义数据，格式为：0x88+FUN+LEN+DATA+SUM
*	SUM等于从该数据帧第一字节开始，也就是帧头(0x88)开始，至该帧数据的最后一字节所有字节的和，只保留低八位(u8).
*	协议中长度字节LEN表示该数据帧内包含数据的字节总长度，不包括帧头、功能字、长度字节和最后的校验位，只是数据的字节长度和。
*			比如该帧数据内容为3个int16型数据，那么LEN等于6
*	帧格式：0x88+0xAF+0x1C+ACC DATA+GYRO DATA+MAG DATA+ANGLE DATA+ 0x00 0x00 + 0x00 0x00+SUM，共32字节
*	ACC/GYRO/MAG/ANGLE(roll/pitch/yaw)数据为int16格式
*	其中ANGLE的roll和pitch数据为实际值乘以100以后得到的整数值，yaw为乘以10以后得到的整数值，上位机在显示时再除以100和10.
************************************************************************************************/
void ANO_IMU(void)
{
	u8 i,arr[32],sum=0;	
//	IMU_Update();		//姿态更新咯
	IMU.ROLL*=100;
	IMU.PITCH*=100;
	IMU.YAW*=10;
	
	arr[0]=0X88;					//帧头0x88
	arr[1]=0XAF;					//FUN
	arr[2]=0X1C;					//LEN
	arr[3]=((short)IMU.ax>>8)&0xFF;		//AX
	arr[4]=((short)IMU.ax)&0xFF;
	arr[5]=((short)IMU.ay>>8)&0xFF;		//AY
	arr[6]=((short)IMU.ay)&0xFF;
	arr[7]=((short)IMU.az>>8)&0xFF;		//AZ
	arr[8]=((short)IMU.az)&0xFF;
	arr[9]=((short)IMU.gx>>8)&0xFF;		//GX
	arr[10]=((short)IMU.gx)&0xFF;
	arr[11]=((short)IMU.gy>>8)&0xFF;	//GY
	arr[12]=((short)IMU.gy)&0xFF;
	arr[13]=((short)IMU.gz>>8)&0xFF;	//GZ
	arr[14]=((short)IMU.gz)&0xFF;
	arr[15]=0;												//MX	
	arr[16]=0;
	arr[17]=0;												//MY	
	arr[18]=0;
	arr[19]=0;												//MZ	
	arr[20]=0;
	arr[21]=((short)IMU.ROLL)>>8&0xFF;		//ROLL
	arr[22]=((short)IMU.ROLL)&0xFF;
	arr[23]=((short)IMU.PITCH)>>8&0xFF;	//PITCH
	arr[24]=((short)IMU.PITCH)&0xFF;
	arr[25]=((short)IMU.YAW)>>8&0xFF;			//YAW
	arr[26]=((short)IMU.YAW)&0xFF;
	arr[27]=arr[28]=arr[29]=arr[30]=0;	//0x0000 0000;
	
	for(i=0;i<31;i++)	sum+=arr[i];		//和校验
	arr[31]=sum;
	for(i=0;i<32;i++)									//按协议发送数据
	{	
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
		USART_SendData(USART1,arr[i]);  
	}
}










