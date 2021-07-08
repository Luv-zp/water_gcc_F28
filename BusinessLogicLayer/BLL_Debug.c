#include "BLL_Debug.h"
#include "BLL_Alarm.h"
#include "BLL_Modbus_server.h"
#include "userApp.h"

static void Debug_init(DebugProcessor * const me);
static void Debug_evInit(void);
static void Debug_evStart(void);
static void Debug_evStop(void);
static void Debug_evCylPosU(u8 type);
static void Debug_evGrat(u8 type);
static void Debug_evSetSingleV(void * pData);
static void Debug_evSetProporV(void * pData);
static void Debug_evTimeOut(void);

static void enterDEBUG_OFF(DebugProcessor * const me);
static void enterDEBUG_ON(DebugProcessor * const me);
static void enterDEBUG_CYL_UP(DebugProcessor * const me);

static void exitDEBUG_OFF(DebugProcessor * const me);
static void exitDEBUG_ON(DebugProcessor * const me);
static void exitDEBUG_CYL_UP(DebugProcessor * const me);





static DebugProcessor debugProcessor;
static u8 uCylLockState;
static u8 dCylLockState;


DebugProcessor * DebugProcessor_create(void)
{
	static DebugProcessor * me = 0;
	if(me == 0)
	{
		me = &debugProcessor;
		me->init = Debug_init;
		me->evInit = Debug_evInit;
		me->evstart = Debug_evStart;
		me->evStop = Debug_evStop;
		me->evCylPosU = Debug_evCylPosU;
		me->evGrat = Debug_evGrat;
		me->evSetSingleV = Debug_evSetSingleV;
		me->evSetProporV = Debug_evSetProporV;
		me->evTimeOut = Debug_evTimeOut;
		
		me->pValve = ValveIODev_create();
		me->pExitSign = ExitSignal_create();
		me->pProporV = ProporValDev_create();
		me->pOutTimer = DelayTimer_create(D_TIM_2);
		
		me->isInitFlag = 0;
	}
	return me;
}

static void Debug_init(DebugProcessor * const me)
{
	if(me->isInitFlag == 0)
	{
		me->isInitFlag = 1;
	}
}

//初始化事件，用来进入默认伪状态
static void Debug_evInit(void)
{
	DebugProcessor * me = &debugProcessor;
	me->stateID = DEBUG_OFF;
	enterDEBUG_OFF(me);
}

//启动信号，用来进入DEBUG模式
static void Debug_evStart(void)
{
	DebugProcessor * me = &debugProcessor;
	FlagHand * pErr = errHand_create();	//错误处理对象
	if(DEBUG_OFF == me->stateID)
	{
		exitDEBUG_OFF(me);
		pErr->setVbStatus(EX_DEBUG_S);
		me->stateID = DEBUG_ON;
		enterDEBUG_ON(me);
	}
}

static void Debug_evStop(void)
{
	DebugProcessor * me = &debugProcessor;
	if(me->stateID == DEBUG_ON)
	{
		exitDEBUG_ON(me);
		if(me->pExitSign->getExitSign(me->pExitSign, U_CYL_POS_SIGN))
		{
			me->stateID = DEBUG_OFF;
			enterDEBUG_OFF(me);
		}
		else
		{
			me->stateID = DEBUG_CYL_UP;
			enterDEBUG_CYL_UP(me);
		}
	}
	else if(me->stateID == DEBUG_CYL_UP)
	{
		exitDEBUG_CYL_UP(me);
		me->stateID = DEBUG_OFF;
		enterDEBUG_OFF(me);
	}
}

static void Debug_evCylPosU(u8 type)
{
	DebugProcessor * me = &debugProcessor;
	if(me->stateID == DEBUG_CYL_UP && type == 1)
	{
		exitDEBUG_CYL_UP(me);
		me->stateID = DEBUG_OFF;
		enterDEBUG_OFF(me);
	}
}

static void Debug_evGrat(u8 type)
{
	DebugProcessor * me = &debugProcessor;
	FlagHand * pErr = errHand_create();	//错误处理对象
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
}

//设置单个阀门状态
//pData 指向MODBUS寄存器的指针
static void Debug_evSetSingleV(void * pData)
{
	int i = 0;
	DebugProcessor * me = &debugProcessor;
	if(me->stateID == DEBUG_ON)
	{
		for(i = 0; i < MAX_NUM_V; i ++)
		{
			if((u32)pData & (1 << i))
			{
				if(i == CYL_UP_V || i == CYL_DOWN_V)
				{
					if(!(me->pExitSign->getExitSign(me->pExitSign, GRAT_SIGN)))
					{
						if((u32)pData & 0x80000000)
							me->pValve->setValState(me->pValve, i, 1);
						else
							me->pValve->setValState(me->pValve, i ,0);
					}
				}
				else
				{
					if((u32)pData & 0x80000000)
						me->pValve->setValState(me->pValve, i, 1);
					else
						me->pValve->setValState(me->pValve, i ,0);
				}
			}
		}
	}
}

//设置单个阀门状态
//pData 指向MODBUS寄存器的指针
static void Debug_evSetProporV(void * pData)
{
	DebugProcessor * me = &debugProcessor;
	u32 u32Temp;
	if(me->stateID == DEBUG_ON)
	{
		u32Temp = ((u16 *)pData)[0] << 16 | ((u16 *)pData)[1];
		me->pProporV->setOutputPres(me->pProporV, (*(float *)&u32Temp) / 100.0f);
	}
}

static void Debug_evTimeOut(void)
{
	DebugProcessor * me = &debugProcessor;
	FlagHand * pErr = errHand_create();	//错误处理对象
	if(me->stateID == DEBUG_CYL_UP)
	{
		exitDEBUG_CYL_UP(me);
		pErr->setErrBit(ERR_CYL_U);
		me->stateID = DEBUG_OFF;
		enterDEBUG_OFF(me);
	}
}

static void enterDEBUG_OFF(DebugProcessor * const me)
{
	me->pProporV->setOutputPres(me->pProporV, 0);	//关闭比例阀
}

static void enterDEBUG_ON(DebugProcessor * const me)
{
	AppMessage msg;
	msg.dataType = MB_UPDATE_STATE;
	msg.pVoid = (void *)MB_STATE_DEBUG;
	xQueueSend(modbusQ, &msg, 10);
}

static void enterDEBUG_CYL_UP(DebugProcessor * const me)
{
	int i = 0;
	for(i = 0; i < MAX_NUM_V; i ++)
	{
		if((i == LIT_VACUUM_V) || (i == BIG_LEAK_1_V) || (i == BIG_LEAK_2_V) || (i == BIG_LEAK_3_V) || 
			(i == BIG_LEAK_4_V) || (i == DRY_V) || (i == BALANCE_V) || (i == AIR_IN_V) || (i == PUMP_WATER_V) || (i == CYL_DOWN_V))
		{
			me->pValve->setValState(me->pValve, i, 1);		
		}
		else
			me->pValve->setValState(me->pValve, i, 0);
	}
	me->pOutTimer->setDelayTime(me->pOutTimer, 15);	//定时15s
}

static void exitDEBUG_OFF(DebugProcessor * const me)
{
}

static void exitDEBUG_ON(DebugProcessor * const me)
{
}

static void exitDEBUG_CYL_UP(DebugProcessor * const me)
{
}


