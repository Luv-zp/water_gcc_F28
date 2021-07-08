#include "userApp.h"
#include "universalType.h"
#include "led.h"
#include "BLL_Modbus_server.h"
#include "BLL_MainFsm.h"
#include "lwip_client_app.h"



//LED1任务句柄
//TaskHandle_t Led1Task_Handler;
//通信任务句柄
TaskHandle_t ComTask_Handler;
	
//modbus通信任务队列
QueueHandle_t modbusQ;
//tcp客户端任务队列
QueueHandle_t client_netQ;
//tcp服务器任务队列
QueueHandle_t server_netQ;

//软件定时器句柄
TimerHandle_t adConvT_Handler;

static void adConvestCb(TimerHandle_t xTimer);


//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
				

	//创建LED1任务
//    xTaskCreate((TaskFunction_t )led1_task,             
//                (const char*    )"led1_task",           
//                (uint16_t       )LED1_STK_SIZE,        
//                (void*          )NULL,                  
//                (UBaseType_t    )LED1_TASK_PRIO,        
//                (TaskHandle_t*  )&Led1Task_Handler);
				
	//创建通信任务
    xTaskCreate((TaskFunction_t )com_Task,             
                (const char*    )"com_task",           
                (uint16_t       )COM_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )COM_TASK_PRIO,        
                (TaskHandle_t*  )&ComTask_Handler);
	
	//用于实时数据更新的定时器，更新周期500ms
	adConvT_Handler = xTimerCreate((const char*		)"adConvestCb",
								   (TickType_t		)500,
								   (UBaseType_t		)pdTRUE,
								   (void *			)3,
								   (TimerCallbackFunction_t)adConvestCb);
											  
	//创建消息队列
	modbusQ = xQueueCreate(20, sizeof(AppMessage));
	client_netQ = xQueueCreate(10, sizeof(AppMessage));
	server_netQ = xQueueCreate(10, sizeof(AppMessage));
	  				
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//LED1任务
//void led1_task(void *p_arg)
//{
//	while(1)
//	{
//		LED1_Toggle;
//		vTaskDelay(500);		//延时1000ms
//	}
//}

//通信任务
void com_Task(void *pvParameters)
{
	ModbusDev * me = ModbusDev_create();
	tcp_client_init();
	FiniteStateMachine * pMainFsmDev = FiniteStateMachine_create();
	pMainFsmDev->init(pMainFsmDev);
	xTimerStart(adConvT_Handler, 10);
	me->devRun_thread();
	
}



//统计任务
/*
void stat_Task(void *pvParameters)
{
	UBaseType_t free;
	UBaseType_t used;
	float pres = 0;
	while(1)
	{
		//设备控制堆栈使用情况
		free = uxTaskGetStackHighWaterMark(DeviceTask_Handler);
		used = DEVICE_STK_SIZE - free;
		printf("DeviceTask is			:%d\r\n", (int)(used * 100 / (used + free)));
		//有限状态机堆栈使用情况
		free = uxTaskGetStackHighWaterMark(FsmTask_Handle);
		used = FSM_STK_SIZE - free;
		printf("FsmTask is			:%d\r\n", (int)(used * 100 / (used + free)));
	}
}
*/

static void adConvestCb(TimerHandle_t xTimer)
{
	AppMessage msg;
	DataServer * me = DataServer_create();
	float presTemp = 0;
	static float waterPres;

	presTemp = me->getPresData(me, CUR_WATER_DATA);	//获取水压，单位为kpa
	waterPres = presTemp / 100.0;	//转换为Bar
	
	//给modbus发送消息更新水压和气压数据
	msg.dataType = MB_UPDATE_WATER_PRES;
	msg.pVoid = &waterPres;
	xQueueSend(modbusQ, &msg, 0);
}

