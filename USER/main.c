#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "mpu.h"
#include "pcf8574.h"
#include "malloc.h"
#include "userApp.h"

//��ʼ������
TaskHandle_t StartTask_Handler;

int main(void)
{
	Write_Through();                //Cahceǿ��͸д
    MPU_Memory_Protection();        //������ش洢����
    Cache_Enable();                 //��L1-Cache
	
    HAL_Init();				        //��ʼ��HAL��
    Stm32_Clock_Init(432,25,2,9);   //����ʱ��,216Mhz 
    delay_init(216);                //��ʱ��ʼ��
	uart_init(115200);		        //���ڳ�ʼ��
//    LED_Init();                     //��ʼ��LED 
//	PCF8574_Init();                 //��ʼ��PCF8574
	my_mem_init(SRAMIN);		    //��ʼ���ڲ��ڴ��
	my_mem_init(SRAMEX);		    //��ʼ���ⲿ�ڴ��
	my_mem_init(SRAMDTCM);		    //��ʼ��DTCM�ڴ��
	
	__HAL_RCC_CRC_CLK_ENABLE();		//ʹ��CRCʱ��
	
    //������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������                
    vTaskStartScheduler();          //�����������
}







