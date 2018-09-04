#ifndef __TIM2_INT_H__
#define __TIM2_INT_H__

#include "stm32f10x.h"
#include "sys.h"

void TIM2_Config(void);
void Show_Screen(void);
void LEV_BLE(void);
void MPU_BLE(void);
void LEV_NRF(void);
void MPU_NRF(void);
void NRF_BLE(void);
void BLE_NRF(void);

void DATA_Task(void);


#endif

