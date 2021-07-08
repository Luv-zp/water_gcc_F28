#include "userApp.h"
#include "universalType.h"
#include "led.h"
#include "BLL_Modbus_server.h"
#include "BLL_MainFsm.h"
#include "lwip_client_app.h"



//LED1������
//TaskHandle_t Led1Task_Handler;
//ͨ��������
TaskHandle_t ComTask_Handler;
	
//modbusͨ���������
QueueHandle_t modbusQ;
//tcp�ͻ����������
QueueHandle_t client_netQ;
//tcp�������������
QueueHandle_t server_netQ;

//�����ʱ�����
TimerHandle_t adConvT_Handler;

static void adConvestCb(TimerHandle_t xTimer);


//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
				

	//����LED1����
//    xTaskCreate((TaskFunction_t )led1_task,             
//                (const char*    )"led1_task",           
//                (uint16_t       )LED1_STK_SIZE,        
//                (void*          )NULL,                  
//                (UBaseType_t    )LED1_TASK_PRIO,        
//                (TaskHandle_t*  )&Led1Task_Handler);
				
	//����ͨ������
    xTaskCreate((TaskFunction_t )com_Task,             
                (const char*    )"com_task",           
                (uint16_t       )COM_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )COM_TASK_PRIO,        
                (TaskHandle_t*  )&ComTask_Handler);
	
	//����ʵʱ���ݸ��µĶ�ʱ������������500ms
	adConvT_Handler = xTimerCreate((const char*		)"adConvestCb",
								   (TickType_t		)500,
								   (UBaseType_t		)pdTRUE,
								   (void *			)3,
								   (TimerCallbackFunction_t)adConvestCb);
											  
	//������Ϣ����
	modbusQ = xQueueCreate(20, sizeof(AppMessage));
	client_netQ = xQueueCreate(10, sizeof(AppMessage));
	server_netQ = xQueueCreate(10, sizeof(AppMessage));
	  				
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//LED1����
//void led1_task(void *p_arg)
//{
//	while(1)
//	{
//		LED1_Toggle;
//		vTaskDelay(500);		//��ʱ1000ms
//	}
//}

//ͨ������
void com_Task(void *pvParameters)
{
	ModbusDev * me = ModbusDev_create();
	tcp_client_init();
	FiniteStateMachine * pMainFsmDev = FiniteStateMachine_create();
	pMainFsmDev->init(pMainFsmDev);
	xTimerStart(adConvT_Handler, 10);
	me->devRun_thread();
	
}



//ͳ������
/*
void stat_Task(void *pvParameters)
{
	UBaseType_t free;
	UBaseType_t used;
	float pres = 0;
	while(1)
	{
		//�豸���ƶ�ջʹ�����
		free = uxTaskGetStackHighWaterMark(DeviceTask_Handler);
		used = DEVICE_STK_SIZE - free;
		printf("DeviceTask is			:%d\r\n", (int)(used * 100 / (used + free)));
		//����״̬����ջʹ�����
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

	presTemp = me->getPresData(me, CUR_WATER_DATA);	//��ȡˮѹ����λΪkpa
	waterPres = presTemp / 100.0;	//ת��ΪBar
	
	//��modbus������Ϣ����ˮѹ����ѹ����
	msg.dataType = MB_UPDATE_WATER_PRES;
	msg.pVoid = &waterPres;
	xQueueSend(modbusQ, &msg, 0);
}

