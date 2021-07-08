#ifndef _USER_APP_H
#define _USER_APP_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "rtos_timers.h"
#include "event_groups.h"
#include "sys.h"

#ifdef 	MY_OS_GLOBALS
#define MY_OS_EXT
#else
#define MY_OS_EXT 	extern
#endif

#if defined(__cplusplus)
extern "C" {
#endif

MY_OS_EXT QueueHandle_t client_netQ;
MY_OS_EXT QueueHandle_t server_netQ;	

//操作系统启动任务
#define START_TASK_PRIO			25
#define START_STK_SIZE 			256  
MY_OS_EXT TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);	

//LED1任务

#define LED1_TASK_PRIO 			2
#define LED1_STK_SIZE			256
MY_OS_EXT TaskHandle_t Led1Task_Handler;
void led1_task(void *pvParameters);

#ifdef __cplusplus
}
#endif
	
#endif

