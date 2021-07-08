#include "FML_DelayTime.h"

static void DelayTimer_init(DelayTimer * me);
static void DelayTimer_setDelayTime(DelayTimer * me, float time);
static void DelayTimer_stopTimer(DelayTimer * me);
static void DelayTimer_registerCallback(DelayTimer * me, void * pInstance, const pGeneralCb p, DelayTimeCbType type);
static void DelayTimer_unregisterCallback(DelayTimer * me, DelayTimeCbType type);
static void delayTimeCb(TimerHandle_t xTimer);

static DelayTimer delayTimer[D_TIM_MAX];
static TimerHandle_t delay_timerHandle[D_TIM_MAX];
static NotifyHandle delayTimer_cbHandle[D_TIM_MAX][TIME_CB_MAX];	//ͨ�þ�����飬���ڱ���ص������͵��ö���


DelayTimer * DelayTimer_create(DelayTimeType timType)
{
	DelayTimer * me = &delayTimer[timType]; 		//ָ��̬����(��static)
	int i = 0;
	if(me->init == 0)
	{
		me = &delayTimer[timType];
		me->init = DelayTimer_init;
		me->setDelayTime = DelayTimer_setDelayTime;
		me->stopTimer = DelayTimer_stopTimer;
		me->registerCallback = DelayTimer_registerCallback;
		me->unregisterCallback = DelayTimer_unregisterCallback;
		me->isInitFlag = 0;
		me->timerType = timType;
		me->timerHandle = delay_timerHandle[timType];
		//����̬�����֪ͨ�����ַ���ݸ������Ա
		for(i = 0; i< TIME_CB_MAX; i++)
		{
			me->cbHandle[i] = &delayTimer_cbHandle[timType][i];
		}
		me->init(me); 
	}
	return me;
}

static void DelayTimer_init(DelayTimer * me)
{
	if(me->isInitFlag == 0)
	{
		me->timerHandle = xTimerCreate((const char * )"timerCb",
									   (TickType_t   )10,
									   (UBaseType_t  )pdTRUE,
									   (void *		 )me,
									   (TimerCallbackFunction_t)delayTimeCb);
		me->isInitFlag = 1;
	}
}

static void DelayTimer_setDelayTime(DelayTimer * me, float time)
{
	me->delayTime = time;
//	taskYIELD();	//����һ���������
	if(pdFALSE == xTimerIsTimerActive(me->timerHandle))
	{
		xTimerStart(me->timerHandle, 10);
		me->nowTime = 0;
	}
}

static void DelayTimer_stopTimer(DelayTimer * me)
{
//	taskYIELD();
	if(pdFALSE != xTimerIsTimerActive(me->timerHandle))
	{
		xTimerStop(me->timerHandle, 10);
	}
}

static void DelayTimer_registerCallback(DelayTimer * me, void * pInstance, const pGeneralCb p, DelayTimeCbType type)
{
	me->cbHandle[type]->callback = p;
	me->cbHandle[type]->pInstance = pInstance;
}

static void DelayTimer_unregisterCallback(DelayTimer * me, DelayTimeCbType type)
{
	me->cbHandle[type]->callback = 0;
	me->cbHandle[type]->pInstance = 0;
}

static void delayTimeCb(TimerHandle_t xTimer)
{
	DelayTimer * me = (DelayTimer *)pvTimerGetTimerID(xTimer);
	me->nowTime += 0.01f;
	float dispTime = 0;
	if(me->nowTime >= me->delayTime)
	{
		/*��ʱʱ������趨ʱ�䣬��ֹͣ�ö�ʱ������ִ����ػص�����*/
		xTimerStop(xTimer, 0);
		if((me->cbHandle[TIME_OVER]->callback != 0)&&(me->cbHandle[TIME_OVER]->pInstance != 0))
		{
			dispTime = 0;
			me->cbHandle[TIME_OVER]->callback(me->cbHandle[TIME_OVER]->pInstance , 0);
		}	
		if((me->cbHandle[TIME_DISP]->callback != 0)&&(me->cbHandle[TIME_DISP]->pInstance != 0))
		{
			me->cbHandle[TIME_DISP]->callback(me->cbHandle[TIME_DISP]->pInstance , &dispTime);
		}
	}
	else
	{
		/*����ʱ�����У�ִ����صĻص�����*/
		if((me->cbHandle[TIME_DISP]->callback != 0)&&(me->cbHandle[TIME_DISP]->pInstance != 0))
		{
			dispTime = me->delayTime - me->nowTime;
			me->cbHandle[TIME_DISP]->callback(me->cbHandle[TIME_DISP]->pInstance , &dispTime);
		}
		if((me->cbHandle[TIME_REACT]->callback != 0)&&(me->cbHandle[TIME_REACT]->pInstance != 0))
		{
			me->cbHandle[TIME_REACT]->callback(me->cbHandle[TIME_REACT]->pInstance , 0);
		}
	}
}


