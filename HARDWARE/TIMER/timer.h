#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"

extern TIM_HandleTypeDef TIM3_Handler;      //��ʱ��3��� 
extern TIM_HandleTypeDef TIM4_Handler;		//��ʱ��4���
extern volatile unsigned long long FreeRTOSRunTimeTicks;

void TIM3_PWM_Init(u16 arr, u16 psc); 
void TIM_SetTIM3Compare4(u32 compare);
void TIM4_Init(u16 arr, u16 psc);
void ConfigureTimeForRunTimeStats(void);
#endif

