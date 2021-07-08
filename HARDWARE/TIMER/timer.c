#include "timer.h"
#include "led.h"
#include "usart.h"
#include "malloc.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"



TIM_HandleTypeDef TIM3_Handler;     //��ʱ����� 
TIM_HandleTypeDef TIM4_Handler;     //��ʱ����� 

//FreeRTOSʱ��ͳ�����õĽ��ļ�����
volatile unsigned long long FreeRTOSRunTimeTicks = 0;

//��ʼ��TIM4ʹ��ΪFreeRTOS��ʱ��ͳ���ṩʱ��
void ConfigureTimeForRunTimeStats(void)
{
	//��ʱ��4��ʼ������ʱ��ʱ��Ϊ108M����Ƶϵ��Ϊ108-1�����Զ�ʱ��4��Ƶ��
	//Ϊ108M/108=1M���Զ���װ��Ϊ50-1����ô��ʱ�����ھ���50us
	FreeRTOSRunTimeTicks=0;
	TIM4_Init(50-1,108-1);	//��ʼ��TIM4
}

//ͨ�ö�ʱ��4�жϳ�ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��4!(��ʱ��3����APB1�ϣ�ʱ��ΪHCLK/2)
void TIM4_Init(u16 arr,u16 psc)
{  
    TIM4_Handler.Instance=TIM4;                          //ͨ�ö�ʱ��4
    TIM4_Handler.Init.Prescaler=psc;                     //��Ƶ
    TIM4_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM4_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM4_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//ʱ�ӷ�Ƶ����
    HAL_TIM_Base_Init(&TIM4_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM4_Handler); //ʹ�ܶ�ʱ��4�Ͷ�ʱ��4�����жϣ�TIM_IT_UPDATE    
}

//PWM��ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
TIM_OC_InitTypeDef TIM3_CH4Handler;
void TIM3_PWM_Init(u16 arr, u16 psc)
{
	TIM3_Handler.Instance=TIM3;                          //ͨ�ö�ʱ��3
    TIM3_Handler.Init.Prescaler=psc;                     //��Ƶ
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM3_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//ʱ�ӷ�Ƶ����
	HAL_TIM_PWM_Init(&TIM3_Handler);		//��ʼ��PWM
	
	TIM3_CH4Handler.OCMode = TIM_OCMODE_PWM1;		//ģʽ����PWM1
	TIM3_CH4Handler.Pulse = arr / 2;		//���ñȽ�ֵ����ֵ����ȷ��ռ�ձ�
	TIM3_CH4Handler.OCPolarity = TIM_OCPOLARITY_LOW;	//����Ƚϼ���λ��
	HAL_TIM_PWM_ConfigChannel(&TIM3_Handler, &TIM3_CH4Handler, TIM_CHANNEL_4);	//����TIM3ͨ��4
	HAL_TIM_PWM_Start(&TIM3_Handler, TIM_CHANNEL_4);
}

//����ռ�ձ�
void TIM_SetTIM3Compare4(u32 compare)
{
	TIM3->CCR4 = compare;
}

//��ʱ���ײ�����������ʱ�ӣ���������
//�˺����ᱻHAL_TIM_PWM_Init()��������
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef * htim)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_TIM3_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_Initure.Pin = GPIO_PIN_1;
	GPIO_Initure.Mode = GPIO_MODE_AF_PP;	//�������
	GPIO_Initure.Pull = GPIO_PULLUP;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;
	GPIO_Initure.Alternate = GPIO_AF2_TIM3;  //PB1����ΪTIM3_CH4
	HAL_GPIO_Init(GPIOB, &GPIO_Initure);
}


//��ʱ���ײ�����������ʱ�ӣ������ж����ȼ�
//�˺����ᱻHAL_TIM_Base_Init()��������
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();            //ʹ��TIM3ʱ��
		HAL_NVIC_SetPriority(TIM3_IRQn,8,0);    //�����ж����ȼ�����ռ���ȼ�8�������ȼ�0
		HAL_NVIC_EnableIRQ(TIM3_IRQn);          //����ITM3�ж�   
	}  
	if(htim->Instance==TIM4)
	{
		__HAL_RCC_TIM4_CLK_ENABLE();            //ʹ��TIM3ʱ��
		HAL_NVIC_SetPriority(TIM4_IRQn,9,0);    //�����ж����ȼ�����ռ���ȼ�8�������ȼ�0
		HAL_NVIC_EnableIRQ(TIM4_IRQn);          //����ITM3�ж�   
	}  
}

//��ʱ��4�жϷ�����
void TIM4_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM4_Handler);
}

//��ʱ��4�жϷ���������
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim==(&TIM4_Handler))
    {
        FreeRTOSRunTimeTicks++;
    }
}
