/**
  ******************************************************************************
  * @file    my_usart.h
  * @author  LiuSongHu_QQ2580065056
  * @version V1.0.0
  * @date    20116-8-21
  * @brief   This file contains all the functions prototypes for the my_usart 
  *          firmware library.
  ******************************************************************************
  * @attention
  *
	*	Use USART1 of APB2	
	*********************************************************************************
	*	1.修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
	*	2.增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)	
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef	__USART_H__
#define __USART_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>


#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	

void USART1_Config(u32);

#endif
