/************************************************************
*		SSD1306 SLAVE ADDRESS      0 1 1 1   1 0	SA0   R/W#
*		Control byte			Co	D/C	0	0		0	0	0	0 	ACK
*************************************************************/
#include "OLED.h"
#include "FONT.h"
#include "PIC.h"

/**
	*	@brief	OLED send command
	*	@param	cmd
	*	@retval None
	*/
void OLED_CMD(u8 cmd)
{
	SIM_IIC_START(); 									//START
	SIM_SEND_BYTE(Device_addr);				//DEVICE_ADDRESS & WRITE
	SIM_IIC_CHECK_ACK();							//ACK
	SIM_SEND_BYTE(0x00);							//COMMAND_ADDRESS
	SIM_IIC_CHECK_ACK();							//ACK	//	while(length--)
	SIM_SEND_BYTE(cmd);								//DATA
	SIM_IIC_MASTER_ACK(NACK);					//NACK
	SIM_IIC_STOP();										//STOP
}
/**
	*	@brief  OLED send data to fill the grid
	*	@param  data
	*	@retcal None
	*/
void OLED_DATA(u8 data)
{
	SIM_IIC_START(); 									//START
	SIM_SEND_BYTE(Device_addr);				//DEVICE_ADDRESS & WRITE
	SIM_IIC_CHECK_ACK();							//ACK
	SIM_SEND_BYTE(0x40);							//DATA_ADDRESS
	SIM_IIC_CHECK_ACK();							//ACK	//	while(length--)
	SIM_SEND_BYTE(data);							//DATA
	SIM_IIC_MASTER_ACK(NACK);					//NACK
	SIM_IIC_STOP();										//STOP
}
/**
	*	@brief	OLED Initialized
	*	@param  None
	*	@retval None
	*/
void OLED_Init(void)
{
	Delay_ms(200);
	OLED_CMD(0xAE);				//显示关闭
	OLED_CMD(0x21);OLED_CMD(0x00);OLED_CMD(0x7F);				//设置列起始地址和结束地址	
	OLED_CMD(0x22);OLED_CMD(0x00);OLED_CMD(0x07);				//设置页起始地址和结束地址
	OLED_CMD(0x20);OLED_CMD(0x00);				//设置内存寻址模式  00->水平寻址，	01->垂直寻址
	OLED_CMD(0x40);				//复位后起始行地址为0	
	OLED_CMD(0xA1);				//列地址127映射到SEG0
	OLED_CMD(0xC8);				//扫描在重映射模式
	OLED_CMD(0xD3);OLED_CMD(0x00);				//设置COM口垂直移动
	OLED_CMD(0xDA);OLED_CMD(0x10);				//设置COM脚为可选择COM脚配置，禁用COM左右重映射
	OLED_CMD(0xA8);OLED_CMD(0x3F);				//设置MUX比率
	OLED_CMD(0xDB);OLED_CMD(0x30);				//设置VCOMH电压
	OLED_CMD(0x81);OLED_CMD(0xFF);				//设置对比度	
	OLED_CMD(0xA4);				//内存内容显示
	OLED_CMD(0xA6);				//正常显示
	OLED_CMD(0xFF);				//亮度最高	
	OLED_CMD(0x8D);				//电荷泵设置
	OLED_CMD(0x14);				//使能电荷泵	
	OLED_CMD(0xAF);				//显示开启	
}
void Start_Pic(void)
{
	/*	开机动画咯	*/	
	OLED_Clear();			//清除显示
	OLED_PIC(KISS);OLED_CMD(0xA7);Delay_ms(1000);		//反白显示图片//延时5s
	OLED_CMD(0xA6);Delay_ms(1000);		//正常显示

	OLED_Clear();			//清除显示
}

/**
	*	@brief  OLED screen clear
	*	@param  None
	*	@retval None
	*/
void OLED_Clear(void)
{
	u8 i,n;
	for(i=0;i<8;i++)
	{
		OLED_CMD(0xB0+i);		//页地址
		OLED_CMD(0x00);			//低位列起始地址
		OLED_CMD(0x10);			//高位列起始地址
		for(n=0;n<128;n++)
			OLED_DATA(0x00);
	}
}
/**
	*	@brief  OLED area screen clear(page3 to page 8)
	*	@param  None
	*	@retval None
	*/
void OLED_Area_Clear(void)
{
	u8 i,n;
	for(i=3;i<8;i++)
	{
		OLED_CMD(0xB0+i);		//页地址
		OLED_CMD(0x00);			//低位列起始地址
		OLED_CMD(0x10);			//高位列起始地址
		for(n=0;n<128;n++)
			OLED_DATA(0x00);
	}
}
/**
	*	@brief  OLED Screen Set site
	*	@param  x , y
	*	@retval None
	*/
void OLED_Pos(u8 x,u8 y)
{
	u8 ly,hy,hx;//,lx;
	x>=64?x%=64:x;
	y>=128?y%=128:y;
	ly=y%16;		//求余
	hy=y/16;		//求模
//	lx=x%8;
	hx=x/8;
	OLED_CMD(0xB0+hx);		//页地址
	OLED_CMD(0x00+ly);		//低位列起始地址
	OLED_CMD(0x10+hy);		//高位列起始地址
//	OLED_DATA(0x01<<lx);		//画点测试
}
/**
	*	@breif  OLED Write size 6*8 byte
	*	@param  x,y,byte
	*	@retval None
	*/
void OLED_C6X8(u8 x,u8 y,u8 byte)
{
	u8 i;
 	u8 chr=byte-' ';
	OLED_Pos(x,y);
	for(i=0;i<6;i++)
		OLED_DATA(F6X8[chr][i]);
}
/**
	*	@breif  OLED Write size 6*8 String
	*	@param  x,y,byte
	*	@retval None
	*/
void OLED_S6X8(u8 x,u8 y,u8 *string)
{
	u8 len=0;
	OLED_Pos(x,y);
	while(string[len]!='\0')	
	{	
		OLED_C6X8(x,y+6*len,string[len]);
		len++;
	}
}
/**
	*	@breif  OLED fill grid to draw Picture
	*	@param  const Pic
	*	@retval None
	*/
void OLED_PIC(const u8 *Pic)
{
	u8 i,j;
	u16 n=0;
	OLED_Pos(0,0);
	for(i=0;i<8;i++)
	{			
		for(j=0;j<128;j++)	
		{		
			OLED_Pos(i*8,j);
			OLED_DATA(Pic[n++]);
		}
	}	
}
/**
	*	@breif  OLED Write size 8*16 byte
	*	@param  x,y,byte
	*	@retval None
	*/
void OLED_C8X16(u8 x,u8 y,u8 byte)
{
	u8 i,j;
 	u8 chr=byte-' ';
	OLED_Pos(x,y);
	for(i=0;i<2;i++)
	{
		OLED_Pos(x+i*8,y);
		for(j=0;j<8;j++)
			OLED_DATA(F8X16[chr][i*8+j]);
	}
}
/**
	*	@breif  OLED Write size 8*16 string
	*	@param  x,y,byte
	*	@retval None
	*/
void OLED_S8X16(u8 x,u8 y,u8 *string)
{
	u8 len=0;
	while(string[len]!='\0')	
	{	
		OLED_C8X16(x,y+len*8,string[len]);
		len++;
	}
}
/*****************************************************
*		m^n函数
*		返回：m^n次方
******************************************************/
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
/*****************************************************
*	x,y :起点坐标	 
*	len :数字的位数
*	mode:模式	0,填充模式(有0省略);1,叠加模式(有0填充)
*	num:数值(0~4294967295)	 
*****************************************************/
void OLED_S6X8Num(u8 x,u8 y,u32 num,u8 len,u8 mode)
{         	
	u8 t,temp;					   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(mode==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_C6X8(x,y+6*t,' ');//空格的ASCII码偏移值为32
				continue;
			}
			else mode=1; 		 	 
		}
		OLED_C6X8(x,y+6*t,temp+'0'); //填充空格
	}
} 




