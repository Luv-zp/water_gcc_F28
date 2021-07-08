#include "BLL_MainFsm.h"
#include "BLL_Alarm.h"
#include "FML_ExtGrat.h"
#include "universalType.h"
#include "userApp.h"

//主状态机任务句柄
TaskHandle_t FsmTask_Handle;
//主状态机任务队列
QueueHandle_t fsmQueue_Handle;


static void Fsm_init(FiniteStateMachine * const me);
static void Fsm_sendEvent(AppMessage * msg);

static void recvExitSignCb(void * pInstance, void * pData);
static void delayOverCb(void * pInstance, void * pData);
static void timeOutCb(void * pInstance, void * pData);

static void mainFsm_eventDispatchThread(void * pvParameters);	//事件处理线程

static FiniteStateMachine mainFsm;
static ParaSetting_t paraSetting;



FiniteStateMachine * FiniteStateMachine_create(void)
{
	static FiniteStateMachine * me = 0;
	if(me == 0)
	{
		me = &mainFsm;
		me->init = Fsm_init;
		me->sendEvent = Fsm_sendEvent;
		me->its_testProcessor = TestProcessor_create();
		me->its_debugProcessor = DebugProcessor_create();
		me->its_dryProcessor = DryProcessor_create();
		me->isInitFlag = 0;
	}
	return me;
}

//主状态机初始化，主要是给测试机传递相应的设备和参数指针
static void Fsm_init(FiniteStateMachine * const me)
{
	ExitSignal * pExitSignal = ExitSignal_create();
	ValveIODev * pValveIODev = ValveIODev_create();
	ProporValDev * pProporValDev = ProporValDev_create();
	DataServer * pDataServer = DataServer_create();
	DelayTimer * pDelayTimer = DelayTimer_create(TIME_DISP_TIM);
	DelayTimer * pOutTimer = DelayTimer_create(TIME_OUT_TIM);
	if(me->isInitFlag == 0)
	{
		me->its_testProcessor->timePara = &paraSetting.paraTime;
		me->its_testProcessor->presPara = &paraSetting.paraPres;
		me->its_testProcessor->valveDev = pValveIODev;
		me->its_testProcessor->adDev = pDataServer;
		me->its_testProcessor->delayTimer = pDelayTimer;
		me->its_testProcessor->outTimer = pOutTimer;
		me->its_testProcessor->proporV = pProporValDev;
		me->its_testProcessor->exitSign = pExitSignal;
		me->its_testProcessor->init(me->its_testProcessor);
		
		me->its_debugProcessor->pValve = pValveIODev;
		me->its_debugProcessor->pExitSign = pExitSignal;
		me->its_debugProcessor->pOutTimer = pOutTimer;
		me->its_debugProcessor->pProporV = pProporValDev;
		me->its_debugProcessor->init(me->its_debugProcessor);
		
		me->its_dryProcessor->pAD = pDataServer;
		me->its_dryProcessor->pValve = pValveIODev;
		me->its_dryProcessor->pExitSign = pExitSignal;
		me->its_dryProcessor->pDelayTimer = pDelayTimer;
		me->its_dryProcessor->pOutTimer = pOutTimer;
		me->its_dryProcessor->pParaSetting = &paraSetting;
		me->its_dryProcessor->init(me->its_dryProcessor);
		
		/*注册回调函数*/
		pDelayTimer->registerCallback(pDelayTimer, me, delayOverCb, TIME_OVER); //延时结束
		pOutTimer->registerCallback(pOutTimer, me, timeOutCb, TIME_OVER); //超时结束
		pExitSignal->registerCallback(pExitSignal, me, recvExitSignCb, EXT_SIGN_CHANGE);//信号报警
		
		taskENTER_CRITICAL();
		//创建任务
		xTaskCreate((TaskFunction_t )mainFsm_eventDispatchThread,
					(const char *	)"fsmTask",
					(uint16_t       )FSM_STK_SIZE,
					(void *			)me,
					(UBaseType_t	)FSM_TASK_PRIO,
					(TaskHandle_t   )FsmTask_Handle);
		fsmQueue_Handle = xQueueCreate(10, sizeof(AppMessage));
//		fsmEvent_Handle = xEventGroupCreate();
					
		me->mainStateID = STOP_TEST;
		me->its_testProcessor->evInit();
		me->isInitFlag = 1;
		taskEXIT_CRITICAL();
	}
}

static void mainFsm_eventDispatchThread(void * pvParameters)
{
//	FiniteStateMachine * me = &mainFsm;
	FiniteStateMachine * me = (FiniteStateMachine *)pvParameters;
	BaseType_t res;
	AppMessage msg;
	float * pFloat = 0;
	int i = 0;
	u32 u32Temp;
	ExitSignal * pExitSign = ExitSignal_create();
	GratDev * pGratDev = GratDev_create();
	ValveIODev * pValve = ValveIODev_create();
	FlagHand * pErr = errHand_create();
	while(1)
	{
		res = xQueueReceive(fsmQueue_Handle, &msg, portMAX_DELAY);
		if(res == pdTRUE)
		{
			switch(msg.dataType)
			{
				case EV_START:
					if(me->mainStateID == STOP_TEST)
					{
						me->its_testProcessor->evStart();
					}
					break;
				case EV_STOP:
					if(me->mainStateID == STOP_TEST)
					{
						me->its_testProcessor->evStop();
					}
					else if((me->mainStateID == STOP_DRY) && (pExitSign->getExitSign(pExitSign, GRAT_SIGN) == 0))
					{
						//给干燥状态机发送停止消息，只有子状态正确停止后，才会恢复到STOP_TEST状态
						me->its_dryProcessor->evStop();
						if(DRY_NULL == me->its_dryProcessor->stateID)
						{
							me->mainStateID = STOP_TEST;
							me->its_testProcessor->evInit();	//测试子状态进入默认伪状态
						}
					}
					break;
				case EV_DELAY_OVER:
					if(me->mainStateID == STOP_TEST)
					{
						me->its_testProcessor->evDelayOver();
					}
					else if(me->mainStateID == STOP_DRY)
					{
						me->its_dryProcessor->evDelayOver();
					}
					break;
				case EV_LIQ_SWITCH:
					if(me->mainStateID == STOP_TEST)
					{
						me->its_testProcessor->evLiqSwitch(*(u8 *)msg.pVoid);
					}
					break;
				case EV_VACUUM:
					if(0 == (u32)msg.pVoid)
					{
						pErr->clearErrBit(ERR_VACUUM);	//真空信号由异常转为正常则清除真空报警
					}
					break;
				case EV_CYL_POS_U:
					if(me->mainStateID == STOP_TEST)
					{
						me->its_testProcessor->evCylPosU(*((u8 *)msg.pVoid));
					}
					else if((me->mainStateID == STOP_DRY) && (pExitSign->getExitSign(pExitSign, GRAT_SIGN) == 0))
					{
						me->its_dryProcessor->evCylPosU(*((u8 *)msg.pVoid));
						if(DRY_NULL == me->its_dryProcessor->stateID)
						{
							me->mainStateID = STOP_TEST;
							me->its_testProcessor->evInit();	//测试子状态进入默认伪状态
						}
					}
					else if((me->mainStateID == STOP_DEBUG) && (pExitSign->getExitSign(pExitSign, GRAT_SIGN) == 0))
					{
						me->its_debugProcessor->evCylPosU(*((u8 *)msg.pVoid));
						if(DEBUG_OFF == me->its_debugProcessor->stateID)
						{
							me->mainStateID = STOP_TEST;
							me->its_testProcessor->evInit();	//测试子状态进入默认伪状态
						}
					}
					break;
				case EV_CYL_POS_D:
					if(me->mainStateID == STOP_TEST)
					{
						me->its_testProcessor->evCylPosD(*((u8 *)msg.pVoid));
					}
					else if(me->mainStateID == STOP_DRY) 
					{
						me->its_dryProcessor->evCylPosU(*((u8 *)msg.pVoid));
					}
					break;
				case EV_GRAT:
					if(*((u8 *)msg.pVoid) == 0)
					{
						pGratDev->trigOff(pGratDev, pValve);
						pErr->clearErrBit(ERR_GRAT);
					}
					else
					{
						pGratDev->trigOn(pGratDev, pValve);
						pErr->setErrBit(ERR_GRAT);
					}
					break;
				case EV_DRY_MODE:
					if((me->mainStateID == STOP_TEST) && 
					   (me->its_testProcessor->getCurrentState(me->its_testProcessor) == STATE_STOP) &&
					   (pExitSign->getExitSign(pExitSign, GRAT_SIGN) == 0))
					{
						me->its_dryProcessor->evInit();
						me->its_dryProcessor->evStart();
						if(DRY_NULL != me->its_dryProcessor->stateID)
						{
							me->mainStateID = STOP_DRY;
						}
					}
					else if((STOP_DRY == me->mainStateID) && (pExitSign->getExitSign(pExitSign, GRAT_SIGN) == 0))
					{
						me->its_dryProcessor->evStop();
						if(DRY_NULL == me->its_dryProcessor->stateID)
						{
							me->mainStateID = STOP_TEST;
							me->its_testProcessor->evInit();
						}
					}
					break;
				case EV_DEBUG_MODE:
					//DEBUG模式切换事件
					if((me->mainStateID == STOP_TEST) && 
					   (me->its_testProcessor->getCurrentState(me->its_testProcessor) == STATE_STOP) &&
					   (pExitSign->getExitSign(pExitSign, GRAT_SIGN) == 0))
					{
						me->its_debugProcessor->evInit();
						me->its_debugProcessor->evstart();
						if(DEBUG_OFF != me->its_debugProcessor->stateID)
						{
							me->mainStateID = STOP_DEBUG;
						}
					}
					else if((STOP_DEBUG == me->mainStateID) && (pExitSign->getExitSign(pExitSign, GRAT_SIGN) == 0))
					{
						me->its_debugProcessor->evStop();
						if(DEBUG_OFF == me->its_debugProcessor->stateID)
						{
							me->mainStateID = STOP_TEST;
							me->its_testProcessor->evInit();
						}
					}
					break;
				case EV_TIME_OUT:
					if(me->mainStateID == STOP_TEST)
					{
						me->its_testProcessor->evTimeOut();
					}
					else if(me->mainStateID == STOP_DRY)
					{
						me->its_dryProcessor->evTimeOut();
					}
					else if(me->mainStateID == STOP_DEBUG)
					{
						me->its_debugProcessor->evTimeOut();
						if(DEBUG_OFF == me->its_debugProcessor->stateID)
						{
							me->mainStateID = STOP_TEST;
							me->its_testProcessor->evInit();
						}
					}
					break;
				case EV_SET_PARA:
					//根据参数设置界面中的数据来更新各种判定限
					pFloat = (float *)(&paraSetting);
					for(i = 0; i < 42; i++)
					{
						u32Temp = (((u16 *)msg.pVoid)[2 * i] << 16) | (((u16 *)msg.pVoid)[2 * i + 1]);
						*(pFloat++) = *((float *)(&u32Temp));
					}	
					paraSetting.paraPres.lowWaterOpening = paraSetting.paraPres.lowWaterOpening / 100.0f;
					paraSetting.paraPres.highWaterOpening = paraSetting.paraPres.highWaterOpening / 100.0f;
					break;
				case EV_SET_SIG_V:
					if(STOP_DEBUG == me->mainStateID)
					{
						me->its_debugProcessor->evSetSingleV(msg.pVoid);
					}
					break;
				default:
					break;
			}
		}
	}
}

static void Fsm_sendEvent(AppMessage * msg)
{
	AppMessage appMsg;
	appMsg.dataType = msg->dataType;
	appMsg.pVoid = msg->pVoid;
	xQueueSend(fsmQueue_Handle, &appMsg, 0);
} 

//用于注册信号改变事件的回调函数
//pInstance 注册回调函数的对象，此处为同步状态机对象
//pData 回调函数需要传递的指针，此处为AppMessage
static void recvExitSignCb(void * pInstance, void * pData)
{
	AppMessage * pMsg = (AppMessage *)pData;
	FiniteStateMachine * me = (FiniteStateMachine *)pInstance;
	switch(pMsg->dataType)
	{
		case WATER_DETEC_SIGN:
			pMsg->dataType = EV_LIQ_SWITCH;
			me->sendEvent(pMsg);
			break;
		case VACUUM_SIGN:
			pMsg->dataType = EV_VACUUM;
			me->sendEvent(pMsg);
			break;
		case U_CYL_POS_SIGN:
			pMsg->dataType = EV_CYL_POS_U;
			me->sendEvent(pMsg);
			break;
		case D_CYL_POS_SIGN:
			pMsg->dataType = EV_CYL_POS_D;
			me->sendEvent(pMsg);
			break;
		case GRAT_SIGN:
			pMsg->dataType = EV_GRAT;
			me->sendEvent(pMsg);
			break;
		default:
			break;
	}
}

//注册延时结束时的回调函数
//pInstance 指向注册者对象的指针
//pData 回调函数所传递的数据
static void delayOverCb(void * pInstance, void * pData)
{
	//延时结束，给主状态机发送延时结束事件
	FiniteStateMachine * me = (FiniteStateMachine *)pInstance;
	AppMessage appMsg;
	appMsg.dataType = EV_DELAY_OVER;
	me->sendEvent(&appMsg);
}

static void timeOutCb(void * pInstance, void * pData)
{
	FiniteStateMachine * me = (FiniteStateMachine *)pInstance;
	AppMessage appMsg;
	appMsg.dataType = EV_TIME_OUT;
	me->sendEvent(&appMsg);
}

