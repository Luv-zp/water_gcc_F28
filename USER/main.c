#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "mpu.h"
#include "pcf8574.h"
#include "malloc.h"
#include "userApp.h"

//开始任务句柄
TaskHandle_t StartTask_Handler;

int main(void)
{
	Write_Through();                //Cahce强制透写
    MPU_Memory_Protection();        //保护相关存储区域
    Cache_Enable();                 //打开L1-Cache
	
    HAL_Init();				        //初始化HAL库
    Stm32_Clock_Init(432,25,2,9);   //设置时钟,216Mhz 
    delay_init(216);                //延时初始化
	uart_init(115200);		        //串口初始化
//    LED_Init();                     //初始化LED 
//	PCF8574_Init();                 //初始化PCF8574
	my_mem_init(SRAMIN);		    //初始化内部内存池
	my_mem_init(SRAMEX);		    //初始化外部内存池
	my_mem_init(SRAMDTCM);		    //初始化DTCM内存池
	
	__HAL_RCC_CRC_CLK_ENABLE();		//使能CRC时钟
	
    //创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄                
    vTaskStartScheduler();          //开启任务调度
}







