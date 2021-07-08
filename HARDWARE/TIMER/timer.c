#include "timer.h"
#include "led.h"
#include "usart.h"
#include "malloc.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"



TIM_HandleTypeDef TIM3_Handler;     //定时器句柄 
TIM_HandleTypeDef TIM4_Handler;     //定时器句柄 

//FreeRTOS时间统计所用的节拍计数器
volatile unsigned long long FreeRTOSRunTimeTicks = 0;

//初始化TIM4使其为FreeRTOS的时间统计提供时基
void ConfigureTimeForRunTimeStats(void)
{
	//定时器4初始化，定时器时钟为108M，分频系数为108-1，所以定时器4的频率
	//为108M/108=1M，自动重装载为50-1，那么定时器周期就是50us
	FreeRTOSRunTimeTicks=0;
	TIM4_Init(50-1,108-1);	//初始化TIM4
}

//通用定时器4中断初始化
//arr：自动重装值
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器4!(定时器3挂在APB1上，时钟为HCLK/2)
void TIM4_Init(u16 arr,u16 psc)
{  
    TIM4_Handler.Instance=TIM4;                          //通用定时器4
    TIM4_Handler.Init.Prescaler=psc;                     //分频
    TIM4_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM4_Handler.Init.Period=arr;                        //自动装载值
    TIM4_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//时钟分频因子
    HAL_TIM_Base_Init(&TIM4_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM4_Handler); //使能定时器4和定时器4更新中断：TIM_IT_UPDATE    
}

//PWM初始化
//arr：自动重装值
//psc：时钟预分频数
TIM_OC_InitTypeDef TIM3_CH4Handler;
void TIM3_PWM_Init(u16 arr, u16 psc)
{
	TIM3_Handler.Instance=TIM3;                          //通用定时器3
    TIM3_Handler.Init.Prescaler=psc;                     //分频
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM3_Handler.Init.Period=arr;                        //自动装载值
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//时钟分频因子
	HAL_TIM_PWM_Init(&TIM3_Handler);		//初始化PWM
	
	TIM3_CH4Handler.OCMode = TIM_OCMODE_PWM1;		//模式悬着PWM1
	TIM3_CH4Handler.Pulse = arr / 2;		//设置比较值，此值用来确定占空比
	TIM3_CH4Handler.OCPolarity = TIM_OCPOLARITY_LOW;	//输出比较极性位低
	HAL_TIM_PWM_ConfigChannel(&TIM3_Handler, &TIM3_CH4Handler, TIM_CHANNEL_4);	//配置TIM3通道4
	HAL_TIM_PWM_Start(&TIM3_Handler, TIM_CHANNEL_4);
}

//设置占空比
void TIM_SetTIM3Compare4(u32 compare)
{
	TIM3->CCR4 = compare;
}

//定时器底册驱动，开启时钟，引脚配置
//此函数会被HAL_TIM_PWM_Init()函数调用
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef * htim)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_TIM3_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_Initure.Pin = GPIO_PIN_1;
	GPIO_Initure.Mode = GPIO_MODE_AF_PP;	//推挽输出
	GPIO_Initure.Pull = GPIO_PULLUP;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;
	GPIO_Initure.Alternate = GPIO_AF2_TIM3;  //PB1复用为TIM3_CH4
	HAL_GPIO_Init(GPIOB, &GPIO_Initure);
}


//定时器底册驱动，开启时钟，设置中断优先级
//此函数会被HAL_TIM_Base_Init()函数调用
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();            //使能TIM3时钟
		HAL_NVIC_SetPriority(TIM3_IRQn,8,0);    //设置中断优先级，抢占优先级8，子优先级0
		HAL_NVIC_EnableIRQ(TIM3_IRQn);          //开启ITM3中断   
	}  
	if(htim->Instance==TIM4)
	{
		__HAL_RCC_TIM4_CLK_ENABLE();            //使能TIM3时钟
		HAL_NVIC_SetPriority(TIM4_IRQn,9,0);    //设置中断优先级，抢占优先级8，子优先级0
		HAL_NVIC_EnableIRQ(TIM4_IRQn);          //开启ITM3中断   
	}  
}

//定时器4中断服务函数
void TIM4_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM4_Handler);
}

//定时器4中断服务函数调用
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim==(&TIM4_Handler))
    {
        FreeRTOSRunTimeTicks++;
    }
}
