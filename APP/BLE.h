#ifndef __BLE_H__
#define __BLE_H__

#include "stm32f10x.h"
#include "sys.h"

void BLE_Config(u32 BaudRate);
void BLE_Tx(u16 data);
u16 BLE_Rx(void);
void USART3_IRQHandler(u16 data);

#endif

