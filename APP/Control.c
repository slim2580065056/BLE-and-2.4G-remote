//	@TIM2_INT Interrupt to Control Program
//	中断控制周期20ms

#include "Control.h"

typedef struct IMU_DATA
{
	float q0,q1,q2,q3;									//四元素	
	float ax,ay,az,gx,gy,gz;						//陀螺仪和加速度计的模拟量所对应的数字量
	float ex,ey,ez;											//滤波融合之后的三轴量
	float PITCH;				//仰俯角
	float YAW;					//偏航角
	float ROLL;					//横滚角

}IMU_param;
extern IMU_param IMU;
extern u16 AD_Value[4];		//摇杆的AD采用值
extern u16 MPU_Value[3];	//MPU6050传感器值
u8 ASCII[10]={'0','1','2','3','4','5','6','7','8','9'};			//将数字转化为ASCII码
Bit_Flag Flag={1,0,0,0,0,0,0};
u8 Key_Val;
u8 bit_k;

void TIM2_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	/*	Tout=((arr+1)*(psc+1))/Tclk	*/
	TIM_TimeBaseStructure.TIM_Prescaler=719;         				//预分频：72 000 000 / (719+1) = 1 000 00 Hz
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //计数方式：向上计数
  TIM_TimeBaseStructure.TIM_Period= 1999;            				//计数周期：1 000 00 / (4999 + 1) = 50 Hz = 0.02 s = 20 ms
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;     //时钟不分割	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	
	NVIC_Config(0,0,TIM2_IRQn,2);	
	TIM_ITConfig(TIM2,TIM_IT_Update , ENABLE);	
	TIM_Cmd(TIM2,ENABLE);	
}

void TIM2_IRQHandler(void)		//中断函数尽可能处理一些简单的工作,复杂的程序工作在主函数中调用
{	
	if(TIM_GetFlagStatus(TIM2,TIM_IT_Update)!=RESET)
	{
		Key_Val=Get_Key();				//在中断程序中获取键值
		DATA_Task();							//通过按键获取使能那个数据传输功能，数据传输函数在主函数中调用
//		(0==Key_Val)?Key_Val:printf("%2d  ",Key_Val); 		//打印按键键值
		TIM_ClearFlag(TIM2,TIM_IT_Update);	
	}
}

/**
	*	@breif  摇杆AD采样转换的数据用蓝牙发送，最终采用函数指针进行调用
	*	@param  None
	*	@retval None
	*/
void DATA_Task(void)
{
	u8 static n;	
//	void (*FLB)(void);//	FLB=LEV_BLE;
//	void (*FMB)(void);//	FMB=MPU_BLE;
//	void (*FLN)(void);//	FLN=LEV_NRF;
//	void (*FMN)(void);//	FMN=MPU_NRF;
//	void (*FNB)(void);//	FNB=NRF_BLE;
//	void (*FBN)(void);//	FBN=BLE_NRF;
	if(Flag.SET_Flag==0 && Key_Val==3)		//在没有进入设置功能的情况下按下设置键
	{
		Flag.SET_Flag=1;										//进入设置功能,同时在显示函数中改变为SET		
		OLED_Area_Clear();
	}
	if(Flag.SET_Flag==1 && Key_Val==13)	
	{
		Flag.SET_Flag=0;										//在设置模式下长设置键则退出设置模式
	}
	if(Flag.SET_Flag==1 && Key_Val==2)		//在设置模式下,按键2为选择模式
	{
		switch(++n)
		{
			case 1:		bit_k=0x01;break;//LEV-->BLE
			case 2:		bit_k=0x02;break;//MPU-->BLE
			case 3:		bit_k=0x04;break;//LEV-->NRF
			case 4:		bit_k=0x08;break;//MPU-->NRF
			case 5:		bit_k=0x10;break;//NRF-->BLE
			case 6:		n=0;bit_k=0x20;break;//BLE-->NRF
			default:	bit_k=0xff;break;
		}
	}
	if((bit_k == 0x01) && Key_Val==1) 
	{
//		n=0;
		Flag.SET_Flag=0;
		OLED_Area_Clear();
		Flag.LEV_BLE_Flag=1; Flag.MPU_BLE_Flag=0; Flag.LEV_NRF_Flag=0;
		Flag.MPU_NRF_Flag=0; Flag.NRF_BLE_Flag=0; Flag.BLE_NRF_Flag=0;
	}
	if((bit_k == 0x02) && Key_Val==1) 
	{
//		n=0;
		Flag.SET_Flag=0;
		OLED_Area_Clear();
		Flag.LEV_BLE_Flag=0; Flag.MPU_BLE_Flag=1; Flag.LEV_NRF_Flag=0;
		Flag.MPU_NRF_Flag=0; Flag.NRF_BLE_Flag=0; Flag.BLE_NRF_Flag=0;
	}
	if((bit_k == 0x04) && Key_Val==1) 
	{
//		n=0;
		Flag.SET_Flag=0;
		OLED_Area_Clear();
		Flag.LEV_BLE_Flag=0; Flag.MPU_BLE_Flag=0; Flag.LEV_NRF_Flag=1;
		Flag.MPU_NRF_Flag=0; Flag.NRF_BLE_Flag=0; Flag.BLE_NRF_Flag=0;
	}
	if((bit_k == 0x08) && Key_Val==1) 
	{
	//	n=0;
		Flag.SET_Flag=0;
		OLED_Area_Clear();
		Flag.LEV_BLE_Flag=0; Flag.MPU_BLE_Flag=0; Flag.LEV_NRF_Flag=0;
		Flag.MPU_NRF_Flag=1; Flag.NRF_BLE_Flag=0; Flag.BLE_NRF_Flag=0;
	}
	if((bit_k == 0x10) && Key_Val==1)
	{
//		n=0;
		Flag.SET_Flag=0;
		OLED_Area_Clear();
		Flag.LEV_BLE_Flag=0; Flag.MPU_BLE_Flag=0; Flag.LEV_NRF_Flag=0;
		Flag.MPU_NRF_Flag=0; Flag.NRF_BLE_Flag=1; Flag.BLE_NRF_Flag=0;
	}		
	if((bit_k == 0x20) && Key_Val==1)
	{
//		n=0;
		Flag.SET_Flag=0;
		OLED_Area_Clear();
		Flag.LEV_BLE_Flag=0; Flag.MPU_BLE_Flag=0; Flag.LEV_NRF_Flag=0;
		Flag.MPU_NRF_Flag=0; Flag.NRF_BLE_Flag=0; Flag.BLE_NRF_Flag=1;
	}		
}

/**
	*	@breif  摇杆AD采样转换的数据用蓝牙发送，最终采用函数指针进行调用
	*	@param  None
	*	@retval None
	*/
void LEV_BLE(void)				//摇杆数据蓝牙发送
{
	u8 i;
	u8 static temp[4];
//	OLED_S6X8(24,30,"LEV");OLED_S6X8(24,80,"BLE");		//OLED显示传输方向
	BLE_Tx('L');						//发送发送设备识别码
	OLED_C6X8(56,40,'L');		//OLED显示
	for(i=0;i<4;i++)
	{
		temp[i]=AD_Value[i]/410;		//由于摇杆精度低,所以转化为10个等级精度，采用ASCII码收发
		BLE_Tx(ASCII[temp[i]]);			//每次发送一个转换的ASCII码
		OLED_C6X8(56,48+8*i,ASCII[temp[i]]);	//OLED显示发送的数据
	}
//	if(Key_Val!=0)	BLE_Tx('K'),BLE_Tx((Key_Val/10)+48),BLE_Tx((Key_Val%10)+48);	//蓝牙发送键值
}

/**
	*	@breif  MPU6050解算的姿态数据用蓝牙发送，最终采用函数指针进行调用
	*	@param  None
	*	@retval None
	*/
void MPU_BLE(void)				//MPU6050传感器姿态数据蓝牙发送
{
	u8 i,j;
	u8 static temp[3][3];
//	OLED_S6X8(24,30,"MPU");OLED_S6X8(24,80,"BLE");		//OLED显示传输方向
	BLE_Tx('M');						//采用USART2中断发送
	OLED_C6X8(56,40,'M');		//OLED显示
	IMU_Update();						//姿态更新
	for(i=0;i<3;i++)
	{		
		temp[i][0]=MPU_Value[i]/100;
		temp[i][1]=MPU_Value[i]%100/10;
		temp[i][2]=MPU_Value[i]%10;
		for(j=0;j<3;j++)
		{			
			BLE_Tx(ASCII[temp[i][j]]);
			OLED_C6X8(56,48+8*j+24*i,ASCII[temp[i][j]]);	//OLED显示发送的数据
		}
	}
//	if(Key_Val!=0)	BLE_Tx('K'),BLE_Tx((Key_Val/10)+48),BLE_Tx((Key_Val%10)+48);	//蓝牙发送键值
}

/**
	*	@breif  摇杆AD采样转换的数据用NRF发送，最终采用函数指针进行调用
	*	@param  None
	*	@retval None
	*/
void LEV_NRF(void)							//摇杆数据NRF发送
{
	u8 i,n=100;
	u8 static temp[4],ASC[5];
//	OLED_S6X8(24,30,"LEV");OLED_S6X8(24,80,"NRF");
	NRF_Tx_Mode();
	ASC[0]='L';										//第一个值存放发送设备识别码
	OLED_C6X8(56,40,'L');
	for(i=0;i<4;i++)
	{
		temp[i]=AD_Value[i]/410;		//转化精度
		ASC[i+1]=ASCII[temp[i]];		//NRF是以字符串形式发送，而蓝牙
		OLED_C6X8(56,48+8*i,ASCII[temp[i]]);
	}
	while(n--)
	{
		(NRF_TxPacket(ASC)!=0x20)?n=0,LED3_Set(1):LED3_Set(0);	//等待发送完设备识别码和数据
	}
}

/**
	*	@breif  MPU6050解算的姿态数据用NRF发送，最终采用函数指针进行调用
	*	@param  None
	*	@retval None
	*/
void MPU_NRF(void)				//MPU6050传感器姿态数据NRF发送
{
	u8 i,j,n=100;
	u8 static temp[3][3],ASC[10];
//	OLED_S6X8(24,30,"MPU");OLED_S6X8(24,80,"NRF");		//OLED显示传输方向
	NRF_Tx_Mode();
	ASC[0]='M';							//第一个值存放发送设备识别码
	OLED_C6X8(56,40,'M');		//OLED显示
	IMU_Update();						//姿态更新
	for(i=0;i<3;i++)
	{		
		temp[i][0]=MPU_Value[i]/100;
		temp[i][1]=MPU_Value[i]%100/10;
		temp[i][2]=MPU_Value[i]%10;
		for(j=0;j<3;j++)
		{	
			ASC[3*i+j+1]=ASCII[temp[i][j]];
			OLED_C6X8(56,48+8*j+24*i,ASCII[temp[i][j]]);	//OLED显示发送的数据
		}
	}
	while(n--)
	{
		(NRF_TxPacket(ASC)!=0x20)?n=0,LED3_Set(1):LED3_Set(0);	//等待发送完设备识别码和数据
	}
}

/**
	*	@breif  NRF接收的数据通过BLE发送
	*	@param  None
	*	@retval None
	*/
void NRF_BLE(void)
{
	u8 i,temp_buf[2],buff[3];
//	OLED_S6X8(24,30,"NRF");OLED_S6X8(24,80,"BLE");
	OLED_C6X8(40,40,'N');OLED_C6X8(56,40,'B');
	NRF_RX_Mode(); 									//NRF接收模式
	if(NRF_RxPacket(temp_buf)==0)		//一旦收到信息则显示
	{
		buff[0]='N';									//加入发送设备标识码
		buff[1]=temp_buf[0];
		temp_buf[1]=0;								//加入字符串结束符
		buff[2]=temp_buf[1];
		NRF_WReg(NRF_WRITE_REG + STATUS, 0xff);//清除中断标志位
		NRF_WReg(FLUSH_RX, 0xff);			//清除数据缓冲 RX FIFO
		
	}
	for(i=1;i<3;i++)
	{
		BLE_Tx(ASCII[buff[i]]);
		OLED_C6X8(40,48+8*i,ASCII[buff[i]]);
		OLED_C6X8(56,48+8*i,ASCII[buff[i]]);
	}
//	if(Key_Val!=0)	BLE_Tx('K'),BLE_Tx((Key_Val/10)+48),BLE_Tx((Key_Val%10)+48);	//蓝牙发送键值
}

/**
	*	@breif  BLE接收的数据通过发NRF送
	*	@param  None
	*	@retval None
	*/
void BLE_NRF(void)
{
	u8 i,n=100;
	u8 static temp,ASC[4]={'B','0','0','0'};	//初始化
//	OLED_S6X8(24,30,"BLE");OLED_S6X8(24,80,"NRF");
	OLED_C6X8(40,40,'B');OLED_C6X8(56,40,'N');
	NRF_Tx_Mode();
//	ASC[0]='B';			//发送设备标识符
	temp=USART_ReceiveData(USART2);		//NRF是以字符串形式发送，而蓝牙以字符形式
	ASC[1]=temp/100;
	ASC[2]=temp%100/10;
	ASC[3]=temp%10;
	for(i=1;i<4;i++)	//ASC[0]为发送设备标识符，所以跳过
	{
		ASC[i]=ASCII[ASC[i]];
		OLED_C6X8(40,48+i*8,ASC[i]);		//显示发送数据
		OLED_C6X8(56,48+i*8,ASC[i]);		//显示接收数据
		printf("%d ",ASC[i]);
	}
	printf("\r\n");
	while(n--)
	{
		(NRF_TxPacket(ASC)!=0x20)?n=0,LED3_Set(1):LED3_Set(0);	//等待发送完设备识别码和数据
	}
}

void Show_Screen(void)
{			
	//=======================第一二三行显示标题 REMOTE_CONTROL =========================
	if(Flag.SET_Flag==0)				OLED_S8X16(0,0," REMOTE_CONTROL ");	
	else if(Flag.SET_Flag==1)		OLED_S8X16(0,0,"      SET       ");
															OLED_S6X8(16,0,"==SET=====CH=====OK==");		
	//=======================第四行显示数据传输方向=========================	
	OLED_C6X8(24,10,Switch_Code()+'0');			OLED_S6X8(24,56,"-->");		
	if((Flag.LEV_BLE_Flag==1 && Flag.SET_Flag==0) || bit_k==0x01)			OLED_S6X8(24,30,"LEV"),OLED_S6X8(24,80,"BLE");
	else if((Flag.MPU_BLE_Flag==1 && Flag.SET_Flag==0) || bit_k==0x02)	OLED_S6X8(24,30,"MPU"),OLED_S6X8(24,80,"BLE");
	else if((Flag.LEV_NRF_Flag==1 && Flag.SET_Flag==0) || bit_k==0x04)	OLED_S6X8(24,30,"LEV"),OLED_S6X8(24,80,"NRF");
	else if((Flag.MPU_NRF_Flag==1 && Flag.SET_Flag==0) || bit_k==0x08)	OLED_S6X8(24,30,"MPU"),OLED_S6X8(24,80,"NRF");
	else if((Flag.NRF_BLE_Flag==1 && Flag.SET_Flag==0) || bit_k==0x10)	OLED_S6X8(24,30,"NRF"),OLED_S6X8(24,80,"BLE");
	else if((Flag.BLE_NRF_Flag==1 && Flag.SET_Flag==0) || bit_k==0x20)	OLED_S6X8(24,30,"BLE"),OLED_S6X8(24,80,"NRF");
	else													OLED_S6X8(24,30,"err"),OLED_S6X8(24,80,"err");
	//=======================第七行显示收发的数据===================================
															OLED_S8X16(32,10,"Rx:");OLED_S8X16(48,10,"Tx:");		
	//=======================第八行显示键值===================================
}






