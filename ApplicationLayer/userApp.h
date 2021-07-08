#ifndef _USER_APP_H
#define _USER_APP_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "rtos_timers.h"
#include "event_groups.h"
#include "sys.h"



#if defined(__cplusplus)
extern "C" {
#endif

extern QueueHandle_t modbusQ;
extern QueueHandle_t client_netQ;
extern QueueHandle_t server_netQ;	
extern QueueHandle_t fsmQueue_Handle;	

//����ϵͳ��������
#define START_TASK_PRIO			25
#define START_STK_SIZE 			256  
extern TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);	

//LED1����
//#define LED1_TASK_PRIO 			2
//#define LED1_STK_SIZE			256
//extern TaskHandle_t Led1Task_Handler;
//void led1_task(void *pvParameters);
	
//ͨ������
#define COM_TASK_PRIO			20
#define COM_STK_SIZE			512
extern TaskHandle_t ComTask_Handler;
void com_Task(void *pvParameters);

//ͳ������
//#define STAT_TASK_PRIO			2
//#define STAT_STK_SIZE			128
//extern TaskHandle_t StatTask_Handler;
//void stat_Task(void *pvParameters);

//TCP�ͻ�������
#define TCPCLIENT_TASK_PRIO      15
#define TCPCLIENT_STK_SIZE       256
extern TaskHandle_t TcpClientTask_Handler;

//TCP����������
#define TCPSERVER_TASK_PRIO      11
#define TCPSERVER_STK_SIZE       256
extern TaskHandle_t TcpServerTask_Handler;

//��״̬������
#define FSM_TASK_PRIO	12
#define FSM_STK_SIZE	512
extern TaskHandle_t FsmTask_Handle;

#ifdef __cplusplus
}
#endif
	
#endif

