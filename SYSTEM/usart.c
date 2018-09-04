/**
  ******************************************************************************
  * @file    my_usart.c
  * @author  LiuSongHu_QQ2580065056
  * @version V1.0.0
  * @date    2016-8-21
  * @brief   This file provides all the my_usart firmware functions. 
  ******************************************************************************
  * @attention
  *
	*	Use USART1 of APB2	
  ******************************************************************************
  */
/**
	*	@brief	Usart Init 
	*	@param	BaudRate
	*	@retval	None
	*/
	
#include "usart.h"

//////////////////////////////////////////////////////////////////
//EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	

void USART1_Config(u32 BaudRate)
{
	/*	GPIO_Init / USART_Init / NVIC_Init Stucture	*/
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/*	setting Clock of periph APIO and USART enable	*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);

	/*	set USARTx_TX of GPIO pin/mode/speed	*/
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
	/*	USARTx_TX	setting pinmode as alternate pushpull	*/
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	/*	set USARTx_RX of GPIO pin/mode/speed	*/
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;
	/*	USARTx_TX	setting pinmode as floating	*/
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IN_FLOATING;
	//GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	/* USARTx_Init	Config */
	USART_InitStructure.USART_BaudRate=BaudRate;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
  USART_InitStructure.USART_StopBits=USART_StopBits_1;
  USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_Mode=USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_Init(USART1,&USART_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1; 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;   

	NVIC_Init(&NVIC_InitStructure);
	
	/*	enable USART1	*/
	USART_Cmd(USART1,ENABLE);
	USART_ITConfig(USART1,USART_IT_RXNE, ENABLE);
}  
  
void USART1_IRQHandler(void)
{
	u8 res;	
	if(USART1->SR&(1<<5))//接收到数据	
	{	 
		res=USART1->DR; 
		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0x0d
			{
				if(res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=res;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}  		 									     
	}
} 

/**
	*	@brief	fputc function redifine
	*	@param	None
	*	@retval	None
	*	@call		printf function
	*/

int fputc(int ch,FILE *f)
{
	/*	content of printf send to usart	*/
	USART_SendData(USART1,(u8) ch);
	//while(!(USART1->SR & USART_FLAG_TXE));
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)!=SET);
	return (ch);	
}

/**
	*	@brief	fgetc function redifine
	*	@param	None
	*	@retval	None
	*	@call		printf function
	*/

int fgetc(FILE *f)
{
	/*	waiting for uart1 input data	*/
	while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)!=SET);
	return (u32)USART_ReceiveData(USART1);
}
