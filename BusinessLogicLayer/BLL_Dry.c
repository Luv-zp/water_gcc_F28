#include "BLL_Dry.h"
#include "BLL_Modbus_server.h"
#include "BLL_Alarm.h"
#include "userAPP.h"


static void Dry_init(DryProcessor * const );

static void Dry_evInit(void);
static void Dry_evStart(void);
static void Dry_evStop(void);
static void Dry_evDelayOver(void);
static void Dry_evVacuum(u8 );
static void Dry_evCylPosU(u8 );
static void Dry_evCylPosD(u8 );
static void Dry_evGrat(u8 );
static void Dry_evTimeOut(void);

static void enterDRY_NULL(DryProcessor * const);
static void enterDRY_CYL_DOWN(DryProcessor * const);
static void enterDRY_CYL_UP(DryProcessor * const);
static void enterDRY_AIR_PURGE(DryProcessor * const);
static void enterDRY_AIR_PUMP(DryProcessor * const);
static void enterDRY_LEAK(DryProcessor * const);

static void exitDRY_NULL(DryProcessor * const);
static void exitDRY_CYL_DOWN(DryProcessor * const);
static void exitDRY_CYL_UP(DryProcessor * const);
static void exitDRY_AIR_PURGE(DryProcessor * const);
static void exitDRY_AIR_PUMP(DryProcessor * const);
static void exitDRY_LEAK(DryProcessor * const);



static DryProcessor dryProcessor;
static u32 dryCount;		//干燥计数
static u8 uCylLockState;
static u8 dCylLockState;


DryProcessor * DryProcessor_create(void)
{
	static DryProcessor * me = 0;
	if(me == 0)
	{
		me = &dryProcessor;
		me->init = Dry_init;
		me->evInit = Dry_evInit;
		me->evStart = Dry_evStart;
		me->evStop = Dry_evStop;
		me->evDelayOver = Dry_evDelayOver;
		me->evVacuum = Dry_evVacuum;
		me->evCylPosU = Dry_evCylPosU;
		me->evCylPosD = Dry_evCylPosD;
		me->evGrat = Dry_evGrat;
		me->evTimeOut = Dry_evTimeOut;
		me->pLeakTest = LeakTestProcessor_create();		
		me->mutex = xSemaphoreCreateMutex();	//创建互斥信号量
		me->isInitFlag = 0;
	}
	return me;
}


static void Dry_init(DryProcessor * const me)
{
	if(me->isInitFlag == 0)
	{
		me->pLeakTest->init(me->pLeakTest);
		me->isInitFlag = 1;
	}
}

static void Dry_evInit(void)
{
	DryProcessor * me = &dryProcessor;
	FlagHand * pErr = errHand_create();
//	xSemaphoreTake(me->mutex, portMAX_DELAY);	//锁定
	pErr->clearErrBit(ERR_NEED_DRY);	//进入干燥状态则清除"需要干燥"的错误状态
	me->stateID = DRY_NULL;
	enterDRY_NULL(me);
//	xSemaphoreGive(me->mutex);	//解锁
}

static void Dry_evStart(void)
{
	DryProcessor * me = &dryProcessor;
	FlagHand * pErr = errHand_create();
//	xSemaphoreTake(me->mutex, portMAX_DELAY);	//锁定
	//只有处于待机状态下，才会响应启动信号
	if(me->stateID == DRY_NULL)
	{
		exitDRY_NULL(me);
		pErr->setVbStatus(EX_RUN_S);	//已经进入干燥阶段，设备运行
		//进入气缸下压状态
		me->stateID = DRY_CYL_DOWN;
		enterDRY_CYL_DOWN(me);
	}
//	xSemaphoreGive(me->mutex);	//解锁	
}

static void Dry_evStop(void)
{
	DryProcessor * me = &dryProcessor;
	FlagHand * pErr = errHand_create();
//	xSemaphoreTake(me->mutex, portMAX_DELAY);	//锁定
	switch(me->stateID)
	{
		case DRY_CYL_DOWN:
			exitDRY_CYL_DOWN(me);
			me->stateID = DRY_CYL_UP;
			enterDRY_CYL_UP(me);
			break;
		case DRY_AIR_PURGE:
			exitDRY_AIR_PURGE(me);
			me->stateID = DRY_CYL_UP;
			enterDRY_CYL_UP(me);
			break;
		case DRY_AIR_PUMP:
			exitDRY_AIR_PUMP(me);
			me->stateID = DRY_CYL_UP;
			enterDRY_CYL_UP(me);
			break;
		case DRY_LEAK:
			exitDRY_LEAK(me);
			me->stateID = DRY_CYL_UP;
			enterDRY_CYL_UP(me);
			break;
		case DRY_CYL_UP:
			exitDRY_CYL_UP(me);
			me->stateID = DRY_NULL;
			enterDRY_NULL(me);
			break;
		default:
			break;
	}
//	xSemaphoreGive(me->mutex);	//解锁	
}

static void Dry_evDelayOver(void)
{
	DryProcessor * me = &dryProcessor;
	FlagHand * pErr = errHand_create();
//	float tempPres;
//	xSemaphoreTake(me->mutex, portMAX_DELAY);	//锁定
	switch(me->stateID)
	{
		case DRY_AIR_PURGE:
			exitDRY_AIR_PURGE(me);
			me->stateID = DRY_AIR_PUMP;
			enterDRY_AIR_PUMP(me);
			break;
		case DRY_AIR_PUMP:
			exitDRY_AIR_PUMP(me);
			if(me->pLeakTest->getPressure(me->pLeakTest) < me->pParaSetting->paraPres.dryLimit)
			{
				me->stateID = DRY_CYL_UP;
				enterDRY_CYL_UP(me);
			}
			else
			{
				if(dryCount > 20)
				{
					pErr->setErrBit(ERR_DRY);
					me->stateID = DRY_CYL_UP;
					enterDRY_CYL_UP(me);
				}
				else
				{
					enterDRY_AIR_PUMP(me);
				}
			}
			break;
		case DRY_LEAK:
			exitDRY_LEAK(me);
			me->stateID = DRY_CYL_UP;
			enterDRY_CYL_UP(me);
			break;
		default:
			break;	
	}
//	xSemaphoreGive(me->mutex);	//解锁	
}

static void Dry_evVacuum(u8 type)
{
}

static void Dry_evCylPosU(u8 type)
{
	DryProcessor * me = &dryProcessor;
//	xSemaphoreTake(me->mutex, portMAX_DELAY);	//锁定
	if(me->stateID == DRY_CYL_UP && type == 1)
	{
		exitDRY_CYL_UP(me);
		me->stateID = DRY_NULL;
		enterDRY_NULL(me);
	}
//	xSemaphoreGive(me->mutex);	//解锁			
}

static void Dry_evCylPosD(u8 type)
{
	DryProcessor * me = &dryProcessor;
	FlagHand * pErr = errHand_create();
//	xSemaphoreTake(me->mutex, portMAX_DELAY);	//锁定
	if(me->stateID == DRY_CYL_DOWN && type == 1)
	{
		exitDRY_CYL_DOWN(me);
		me->stateID = DRY_AIR_PUMP;
		enterDRY_AIR_PUMP(me);
		pErr->clearErrBit(ERR_CYL_D);	//此时正常流程，清除气缸错误
	}
//	xSemaphoreGive(me->mutex);	//解锁	
}

static void Dry_evGrat(u8 type)
{
	DryProcessor * me = &dryProcessor;
	FlagHand * pErr = errHand_create();
//	xSemaphoreTake(me->mutex, portMAX_DELAY);	//锁定
	if(type)
	{
		uCylLockState = me->pValve->getValState(me->pValve, CYL_UP_V);
		dCylLockState = me->pValve->getValState(me->pValve, CYL_DOWN_V);
		me->pValve->setValState(me->pValve, CYL_UP_V, 0);
		me->pValve->setValState(me->pValve, CYL_DOWN_V, 0);
		pErr->setErrBit(ERR_GRAT);
	}
	else
	{
		me->pValve->setValState(me->pValve, CYL_UP_V, uCylLockState);
		me->pValve->setValState(me->pValve, CYL_DOWN_V, dCylLockState);
		pErr->clearErrBit(ERR_GRAT);
	}
//	xSemaphoreGive(me->mutex);	//解锁	
}

static void Dry_evTimeOut(void)
{
	DryProcessor * me = &dryProcessor;
	FlagHand * pErr = errHand_create();
//	xSemaphoreTake(me->mutex, portMAX_DELAY);	//锁定
	if(me->stateID == DRY_CYL_DOWN)
	{
		pErr->setErrBit(ERR_CYL_D);
		exitDRY_CYL_DOWN(me);
		me->stateID = DRY_CYL_UP;
		enterDRY_CYL_UP(me);
	}
	else if(me->stateID == DRY_CYL_UP)
	{
		pErr->setErrBit(ERR_CYL_U);
		exitDRY_CYL_UP(me);
		me->stateID = DRY_NULL;
		enterDRY_NULL(me);
	}
//	xSemaphoreGive(me->mutex);	//解锁	
}

static void enterDRY_NULL(DryProcessor * const me)
{	
	dryCount = 0;
	printf("enterDRY_NULL\r\n");
}

static void enterDRY_CYL_DOWN(DryProcessor * const me)
{
	int i = 0;
	AppMessage msg;
	for(i = 0; i < MAX_NUM_V; i++)
	{
		if((i == BALANCE_V) || (i == CYL_UP_V))
		{
			me->pValve->setValState(me->pValve, i, 1);
		}
		else
		{
			me->pValve->setValState(me->pValve, i, 0);
		}
	}
	
	msg.dataType = MB_UPDATE_STATE;
	msg.pVoid = (void *)MB_STATE_DRY_CYL_DOWN;
	xQueueSend(modbusQ, &msg, 10);
	
	me->pOutTimer->setDelayTime(me->pOutTimer, 25);	//定时25s
	printf("enterDRY_CYL_DOWN\r\n");
}


static void enterDRY_CYL_UP(DryProcessor * const me)
{
	int i = 0;
	AppMessage msg;
	for(i = 0; i < MAX_NUM_V; i ++)
	{
		if(i == CYL_DOWN_V)
		{
			me->pValve->setValState(me->pValve, i, 1);		
		}
		else
			me->pValve->setValState(me->pValve, i, 0);
	}
	
	msg.dataType = MB_UPDATE_STATE;
	msg.pVoid = (void *)MB_STATE_DRY_CYL_UP;
	xQueueSend(modbusQ, &msg, 10);
	
	me->pOutTimer->setDelayTime(me->pOutTimer, 25);	//定时25s
	printf("enterDRY_CYL_UP\r\n");
}

static void enterDRY_AIR_PURGE(DryProcessor * const me)
{
	int i = 0;
	AppMessage msg;
	for(i = 0; i < MAX_NUM_V; i ++)
	{
		if((i == BIG_LEAK_1_V) || (i == BIG_LEAK_2_V) || (i == BIG_LEAK_3_V) || (i == BIG_LEAK_4_V) || 
			(i == LIT_LEAK_1_V) || (i == LIT_LEAK_2_V) || (i == LIT_LEAK_3_V) || (i == LIT_LEAK_4_V) || 
			(i == DRY_V) || (i == AIR_OUT_V) || (i == CYL_UP_V))
		{
			me->pValve->setValState(me->pValve, i, 1);		
		}
		else
			me->pValve->setValState(me->pValve, i, 0);
	}
	
	msg.dataType = MB_UPDATE_STATE;
	msg.pVoid = (void *)MB_STATE_DRY_AIR_PURGE;
	xQueueSend(modbusQ, &msg, 10);
	
	me->pDelayTimer->setDelayTime(me->pDelayTimer, 15);	//定时25s
	printf("enterDRY_AIR_PURGE\r\n");
}

static void enterDRY_AIR_PUMP(DryProcessor * const me)
{
	int i = 0;
	AppMessage msg;
	dryCount ++;
	for(i = 0; i < MAX_NUM_V; i ++)
	{
		if((i == BALANCE_V) || (i == CYL_UP_V))
		{
			me->pValve->setValState(me->pValve, i, 1);		
		}
		else
			me->pValve->setValState(me->pValve, i, 0);
	}
	if(xTimerIsTimerActive(me->pLeakTest->timerHandle) == pdFALSE)
	{
		xTimerStart(me->pLeakTest->timerHandle, 10);
	}
	me->pLeakTest->CF28Lpv->StartCycle(me->pLeakTest->CF28Lpv, &me->pLeakTest->MbTcp_ProtFrame);
	msg.dataType = MB_UPDATE_STATE;
	msg.pVoid = (void *)MB_STATE_DRY_AIR_PUMP;
	xQueueSend(modbusQ, &msg, 10);
	
	me->pDelayTimer->setDelayTime(me->pDelayTimer, 30);	//定时30s
	printf("enterDRY_AIR_PUMP\r\n");
}

static void enterDRY_LEAK(DryProcessor * const me)
{
	int i = 0;
	for(i = 0; i < MAX_NUM_V; i ++)
	{
		if((i == LIT_LEAK_1_V) || (i == LIT_LEAK_2_V) || (i == LIT_LEAK_3_V) || (i == LIT_LEAK_4_V) ||  (i == CYL_UP_V))
		{
			me->pValve->setValState(me->pValve, i, 1);		
		}
		else
			me->pValve->setValState(me->pValve, i, 0);
	}
	
	me->pDelayTimer->setDelayTime(me->pDelayTimer, 10);	//定时25s
	printf("enterDRY_LEAK\r\n");
}

static void exitDRY_NULL(DryProcessor * const me)
{
	printf("exitDRY_NULL\r\n");
}

static void exitDRY_CYL_DOWN(DryProcessor * const me)
{
	me->pOutTimer->stopTimer(me->pOutTimer);
	printf("exitDRY_CYL_DOWN\r\n");
}

static void exitDRY_CYL_UP(DryProcessor * const me)
{
	me->pOutTimer->stopTimer(me->pOutTimer);
	printf("exitDRY_CYL_UP\r\n");
}

static void exitDRY_AIR_PURGE(DryProcessor * const me)
{
	me->pDelayTimer->stopTimer(me->pDelayTimer);
	printf("exitDRY_AIR_PURGE\r\n");
}

static void exitDRY_AIR_PUMP(DryProcessor * const me)
{
	me->pDelayTimer->stopTimer(me->pDelayTimer);
	if(xTimerIsTimerActive(me->pLeakTest->timerHandle) != pdFALSE)
	{
		xTimerStop(me->pLeakTest->timerHandle, 10);
	}
	me->pLeakTest->CF28Lpv->StopCycle(me->pLeakTest->CF28Lpv, &me->pLeakTest->MbTcp_ProtFrame);
	printf("exitDRY_AIR_PUMP\r\n");
}

static void exitDRY_LEAK(DryProcessor * const me)
{
	me->pDelayTimer->stopTimer(me->pDelayTimer);
}




