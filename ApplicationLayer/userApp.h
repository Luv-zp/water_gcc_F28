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

//操作系统启动任务
#define START_TASK_PRIO			25
#define START_STK_SIZE 			256  
extern TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);	

//LED1任务
//#define LED1_TASK_PRIO 			2
//#define LED1_STK_SIZE			256
//extern TaskHandle_t Led1Task_Handler;
//void led1_task(void *pvParameters);
	
//通信任务
#define COM_TASK_PRIO			20
#define COM_STK_SIZE			512
extern TaskHandle_t ComTask_Handler;
void com_Task(void *pvParameters);

//统计任务
//#define STAT_TASK_PRIO			2
//#define STAT_STK_SIZE			128
//extern TaskHandle_t StatTask_Handler;
//void stat_Task(void *pvParameters);

//TCP客户端任务
#define TCPCLIENT_TASK_PRIO      15
#define TCPCLIENT_STK_SIZE       256
extern TaskHandle_t TcpClientTask_Handler;

//TCP服务器任务
#define TCPSERVER_TASK_PRIO      11
#define TCPSERVER_STK_SIZE       256
extern TaskHandle_t TcpServerTask_Handler;

//主状态机任务
#define FSM_TASK_PRIO	12
#define FSM_STK_SIZE	512
extern TaskHandle_t FsmTask_Handle;

#ifdef __cplusplus
}
#endif
	
#endif

