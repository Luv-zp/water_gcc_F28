#include "BLL_SubLeakTest.h"
#include "userApp.h"


static void subTest_init(LeakTestProcessor * const me);

/*多事件接收器，单例，故不需要指向对象自身的指针*/
static void subTest_evInit(LeakTestProcessor * const me);
static void subTest_evStart(LeakTestProcessor * const me);
static void subTest_evStop(LeakTestProcessor * const me);
	
static LeakTestType_t subTest_getCurrentState(LeakTestProcessor * me);
static void F28_SendFrame_GetLast(LeakTestProcessor * const me);
static float subTest_getPressure(LeakTestProcessor * me);
static float subTest_getLeak(LeakTestProcessor * me);
static void subTest_getResult(LeakTestProcessor * const me);

LeakTestProcessor leakTestProcessor;

/*F28参数*/
F28_REALTIME_CYCLE F28_RealTime_Cycle;
F28_LAST_RESULT F28_Last_Result;
static void F28_RealTimeCb(TimerHandle_t xTimer);

LeakTestProcessor * LeakTestProcessor_create(void)
{
	static LeakTestProcessor * me = 0;
	if(me == 0)
	{
		me = &leakTestProcessor;
		me->init = subTest_init;
		me->evInit = subTest_evInit;
		me->evStart = subTest_evStart;
		me->evStop = subTest_evStop;
		me->getCurrentState = subTest_getCurrentState;
		me->getPressure = subTest_getPressure;
		me->getLeak = subTest_getLeak;
		me->getResult = subTest_getResult;
		me->SendFrame_GetLast = F28_SendFrame_GetLast;
		me->CF28Lpv = CF28Light_create();
		me->isInitFlag = 0;
	}
	return me;
}

static void subTest_init(LeakTestProcessor * const me)
{
	if(me->isInitFlag == 0)
	{
		me->isInitFlag = 1;
		me->timerHandle = xTimerCreate((const char * )"F28_RealTime",
									   (TickType_t   )500,
									   (UBaseType_t  )pdTRUE,
									   (void *	   	 )me,
									   (TimerCallbackFunction_t)F28_RealTimeCb);	
	}
}

//获取F28正处于的工作状态
static LeakTestType_t subTest_getCurrentState(LeakTestProcessor * me)
{
	me->subState_ID = me->CF28Lpv->realTimeData->wStatus;
	return me->subState_ID;
}

//获取实时测试腔压力
static float subTest_getPressure(LeakTestProcessor * me)
{
	me->realtime_pressureValue = me->CF28Lpv->realTimeData->fPressureValue;
	return me->realtime_pressureValue;
}

//获取实时泄漏值
static float subTest_getLeak(LeakTestProcessor * me)
{
	me->realtime_leakValue = me->CF28Lpv->realTimeData->fLeakValue;
	return me->realtime_leakValue;
}



//获取最近一次的测试结果
static void subTest_getResult(LeakTestProcessor * const me)
{
	me->last_pressureValue = me->CF28Lpv->lastResult->fPressureValue;
	me->last_leakValue = me->CF28Lpv->lastResult->fLeakValue;
}


static void subTest_evInit(LeakTestProcessor * const me)
{
	me->subState_ID = SUB_STATE_READY;
	if(xTimerIsTimerActive(me->timerHandle) == pdFALSE)
	{
		xTimerStart(me->timerHandle, 10);
	}

}

static void subTest_evStart(LeakTestProcessor * const me)
{
	if(me->subState_ID == SUB_STATE_READY)
	{
		me->CF28Lpv->StartCycle(me->CF28Lpv, &me->MbTcp_ProtFrame);
	}
}

static void subTest_evStop(LeakTestProcessor * const me)
{
	if(me->subState_ID != SUB_STATE_READY)
	{
		me->CF28Lpv->StopCycle(me->CF28Lpv, &me->MbTcp_ProtFrame);
		if(xTimerIsTimerActive(me->timerHandle) != pdFALSE)
		{
			xTimerStop(me->timerHandle, 10);
		}
	}
}

static void F28_RealTimeCb(TimerHandle_t xTimer)
{
	LeakTestProcessor * me = LeakTestProcessor_create();
	me->CF28Lpv->Send_ReadRealTimeDataFrame(me->CF28Lpv, &me->MbTcp_ProtFrame);
}

static void F28_SendFrame_GetLast(LeakTestProcessor * const me)
{
	me->CF28Lpv->Send_ReadLastResultFrame(me->CF28Lpv, &me->MbTcp_ProtFrame);
}

