#include "BLL_Test.h"
#include "BLL_Alarm.h"
#include "math.h"
#include "BLL_Modbus_server.h"
#include "userApp.h"

static void TestProcessor_init(TestProcessor * const me);
static TestStateType_t TestProcessor_getCurrentState(TestProcessor * const me);
static void TestProcessor_evInit(void);
static void TestProcessor_evStart(void);
static void TestProcessor_evStop(void);
static void TestProcessor_evDelayOver(void);
static void TestProcessor_evTimeOut(void);		//超时
static void TestProcessor_evLiqSwitch(u8 type);	//液位报警
static void TestProcessor_evCylPosU(u8 type);	//气缸上限位传感器
static void TestProcessor_evCylPosD(u8 type);	//气缸下限位传感器
static void TestProcessor_evCylPosF(u8 type);	//气缸前限位传感器
static void TestProcessor_evCylPosB(u8 type);	//气缸后限位传感器
static void TestProcessor_evGrat(u8 type);	//光栅

void enterSTOP(TestProcessor * const me);
void enterCYLINDER_FORWARD(TestProcessor * const me);
void enterCYLINDER_DOWN(TestProcessor * const me);
void enter13TOR(TestProcessor * const me);
void enterPRODUCT_TEST(TestProcessor * const me);
void enterAIR_PURGE(TestProcessor * const me);
void enterAIR_PUMP(TestProcessor * const me);
void enterIS_DRY(TestProcessor * const me);
void enterWATER_IN(TestProcessor * const me);
void enterADD_PRES(TestProcessor * const me);
void enterLEAK_TEST(TestProcessor * const me);
void enterWATER_OUT(TestProcessor * const me);
void enterWATER_PUMP(TestProcessor * const me);
void enterCYLINDER_UP(TestProcessor * const me);
void enterCYLINDER_BACK(TestProcessor * const me);

void exitSTOP(TestProcessor * const me);
void exitCYLINDER_FORWARD(TestProcessor * const me);
void exitCYLINDER_DOWN(TestProcessor * const me);
void exit13TOR(TestProcessor * const me);
void exitPRODUCT_TEST(TestProcessor * const me);
void exitAIR_PURGE(TestProcessor * const me);
void exitAIR_PUMP(TestProcessor * const me);
void exitIS_DRY(TestProcessor * const me);
void exitWATER_IN(TestProcessor * const me);
void exitADD_PRES(TestProcessor * const me);
void exitLEAK_TEST(TestProcessor * const me);
void exitWATER_OUT(TestProcessor * const me);
void exitWATER_PUMP(TestProcessor * const me);
void exitCYLINDER_UP(TestProcessor * const me);
void exitCYLINDER_BACK(TestProcessor * const me);




static TestProcessor testProcessor;		//测试机(静态)

//创建同步状态机对象
TestProcessor * TestProcessor_create(void)
{
	static TestProcessor * me = 0;
	if(me == 0)
	{
		me = &testProcessor;
		me->init = TestProcessor_init;
		me->getCurrentState = TestProcessor_getCurrentState;
		me->evInit = TestProcessor_evInit;
		me->evStart = TestProcessor_evStart;
		me->evStop = TestProcessor_evStop;
		me->evDelayOver = TestProcessor_evDelayOver;
		me->evTimeOut = TestProcessor_evTimeOut;
		me->evLiqSwitch = TestProcessor_evLiqSwitch;
		me->evCylPosU = TestProcessor_evCylPosU;
		me->evCylPosD = TestProcessor_evCylPosD;
		me->evCylPosF = TestProcessor_evCylPosF;
		me->evCylPosB = TestProcessor_evCylPosB;
		me->evGrat = TestProcessor_evGrat;
		
		me->its_pSubLeakTestProcessor = LeakTestProcessor_create();
		
		me->isInitFlag = 0;
	}
	return me;
}

static void TestProcessor_init(TestProcessor * const me)
{
	if(me->isInitFlag == 0)
	{
		me->its_pSubLeakTestProcessor->init(me->its_pSubLeakTestProcessor);
		me->isInitFlag = 1;
	}
}

static TestStateType_t TestProcessor_getCurrentState(TestProcessor * const me)
{
	return me->stateID;
}

static void TestProcessor_evInit(void)
{
	TestProcessor * me = &testProcessor;
	me->stateID = STATE_STOP;
	enterSTOP(me);
}

static void TestProcessor_evStart(void)
{
	AppMessage msg;
	TestProcessor * me = &testProcessor;
	FlagHand * pErr = errHand_create();
	if(me->stateID == STATE_STOP)
	{
		pErr->clearAllVbRes();	//清除上次测试结果
		if(me->exitSign->getExitSign(me->exitSign, GRAT_SIGN))	//检测光栅是否正常
			pErr->setErrBit(ERR_GRAT);	//故障
		else
			pErr->clearErrBit(ERR_GRAT);	//检测通过，清除故障
		if(me->exitSign->getExitSign(me->exitSign, VACUUM_SIGN))	//检测真空传感器是否正常
			pErr->setErrBit(ERR_VACUUM);	//故障
		else
			pErr->clearErrBit(ERR_VACUUM);	//检测通过，清除故障
		if(me->exitSign->getExitSign(me->exitSign, WATER_DETEC_SIGN))	//检查液位传感器是否正常
			pErr->setErrBit(ERR_LIQ);
		else
			pErr->clearErrBit(ERR_LIQ);
		if(me->exitSign->getExitSign(me->exitSign, U_CYL_POS_SIGN == 0))	//检测气缸磁性开关是否正常
			pErr->setErrBit(ERR_CYL_U);
		else
			pErr->clearErrBit(ERR_CYL_U);
		if(me->exitSign->getExitSign(me->exitSign, D_CYL_POS_SIGN == 1))
			pErr->setErrBit(ERR_CYL_D);
		else
			pErr->clearErrBit(ERR_CYL_D);
		if(pErr->getAllErr()) return; 	//检测到有错误，直接退出		
		
		msg.dataType = MB_TEST_OVER;
		xQueueSend(modbusQ, &msg, 10);
		exitSTOP(me);
		me->stateID = STATE_CYLINDER_DOWN;
		enterCYLINDER_DOWN(me);
	}
}

static void TestProcessor_evStop(void)
{
	TestProcessor * me = &testProcessor;
	FlagHand * pErr = errHand_create();
	static u8 stopCount = 0;
	switch(me->stateID)
	{
		case STATE_STOP:
			stopCount ++;
			if(stopCount == 2)
			{
				pErr->clearAllErr();
				stopCount = 0;
			}
			break;
		case STATE_CYLINDER_UP:
			me->stateID = STATE_STOP;
			enterSTOP(me);
			break;
		case STATE_CYLINDER_FORWARD:
			exitCYLINDER_FORWARD(me);
			me->stateID = STATE_CYLINDER_BACK;
			enterCYLINDER_BACK(me);
			break;
		case STATE_CYLINDER_DOWN:
			exitCYLINDER_DOWN(me);
			me->stateID = STATE_CYLINDER_UP;
			enterCYLINDER_UP(me);
			break;
		case STATE_13TOR:
			exit13TOR(me);
			me->stateID = STATE_CYLINDER_UP;
			enterCYLINDER_UP(me);
			break;
		case STATE_PRODUCT_TEST:
			exitPRODUCT_TEST(me);
			me->stateID = STATE_CYLINDER_UP;
			enterCYLINDER_UP(me);
			break;
		case STATE_AIR_PURGE:
			exitAIR_PURGE(me);
			me->stateID = STATE_CYLINDER_UP;
			enterCYLINDER_UP(me);
			break;
		case STATE_AIR_PUMP:
			exitAIR_PUMP(me);
			me->stateID = STATE_CYLINDER_UP;
			enterCYLINDER_UP(me);
			break;
		case STATE_IS_DRY:
			exitIS_DRY(me);
			me->stateID = STATE_CYLINDER_UP;
			enterCYLINDER_UP(me);
			break;
		case STATE_WATER_IN:
			exitWATER_IN(me);
			me->stateID = STATE_WATER_OUT;
			enterWATER_OUT(me);
			break;
		case STATE_ADD_PRES:
			exitADD_PRES(me);
			me->stateID = STATE_WATER_OUT;
			enterWATER_OUT(me);
			break;
		case STATE_LEAK_TEST:
			/* 给子状态机发送停止消息，只有当子状态机停止后才会进入后续状态 */
			me->its_pSubLeakTestProcessor->evStop(me->its_pSubLeakTestProcessor);
			if(me->its_pSubLeakTestProcessor->getCurrentState(me->its_pSubLeakTestProcessor) == SUB_STATE_READY)
			{
				me->stateID = STATE_WATER_OUT;
				enterWATER_OUT(me);
			}
			break;
		case STATE_WATER_OUT:
			exitWATER_OUT(me);
			me->stateID = STATE_CYLINDER_UP;
			enterCYLINDER_UP(me);
			break;
		case STATE_WATER_PUMP:
			exitWATER_PUMP(me);
			me->stateID = STATE_CYLINDER_UP;
			enterCYLINDER_UP(me);
			break;
		default:
			break;		
	}
}

static void TestProcessor_evDelayOver(void)
{
	TestProcessor * me = &testProcessor;
	FlagHand * pErr = errHand_create();
	float tempF;
	switch(me->stateID)
	{
		case STATE_13TOR:
			exit13TOR(me);
			if(me->exitSign->getExitSign(me->exitSign, VACUUM_SIGN))
			{
				pErr->setErrBit(ERR_PUMP);
				me->stateID = STATE_CYLINDER_UP;
				enterCYLINDER_UP(me);
			}
			else
			{
				me->stateID = STATE_PRODUCT_TEST;
				enterPRODUCT_TEST(me);
			}
			break;
		case STATE_PRODUCT_TEST:
			/* 产品检测倒计时结束后会运行至此，此时根据水压传感器数据来判断产品有无，并决定后续流程 */
			tempF = me->adDev->getPresData(me->adDev, PRO_DEC_WATER);
			exitPRODUCT_TEST(me);
			/*经过实际测试发现，放上零漏件，产品检测时水压约为-50KPa, 撤掉零漏件检测时约为-19KPa*/
			if((tempF < 0) && (me->presPara->existLimit < fabs(tempF)))
			{
				//产品存在，继续运行
				me->stateID = STATE_AIR_PUMP;
				enterAIR_PUMP(me);	//干燥抽气
			}
			else
			{
				me->stateID = STATE_CYLINDER_UP;
				enterCYLINDER_UP(me);
				pErr->setErrBit(ERR_NO_PRODUCT);
			}
			break;
		case STATE_AIR_PURGE:
			exitAIR_PURGE(me);
			me->stateID = STATE_AIR_PUMP;
			enterAIR_PUMP(me);
			break;
		case STATE_AIR_PUMP:
			exitAIR_PUMP(me);
			me->stateID = STATE_IS_DRY;
		    enterIS_DRY(me);
			break;
		case STATE_IS_DRY:
			exitIS_DRY(me);
			if(me->its_pSubLeakTestProcessor->getPressure(me->its_pSubLeakTestProcessor) > me->presPara->dryLimit)
			{
				me->stateID = STATE_CYLINDER_UP;
				enterCYLINDER_UP(me);
				pErr->setErrBit(ERR_NEED_DRY);
			}
			else
			{
				me->stateID = STATE_WATER_IN;
				enterWATER_IN(me);
			}	
			break;
		case STATE_WATER_IN:
			exitWATER_IN(me);
			if(me->exitSign->getExitSign(me->exitSign, WATER_DETEC_SIGN))
			{
				me->stateID = STATE_ADD_PRES;
				enterADD_PRES(me);
			}
			else
			{
				me->stateID = STATE_WATER_OUT;
				enterWATER_OUT(me);
				pErr->setErrBit(ERR_LIQ);
			}
			break;
		case STATE_ADD_PRES:
			exitADD_PRES(me);
			tempF = me->adDev->getPresData(me->adDev, AFTER_ADD_WATER);
			if(me->exitSign->getExitSign(me->exitSign, WATER_DETEC_SIGN) && (tempF > me->presPara->highWaterMin) && (tempF < me->presPara->highWaterMax))
			{
				me->stateID = STATE_LEAK_TEST;
				enterLEAK_TEST(me);
				me->its_pSubLeakTestProcessor->evStart(me->its_pSubLeakTestProcessor);	//泄露测试子流程开始
			}
			else
			{
				if(me->exitSign->getExitSign(me->exitSign, WATER_DETEC_SIGN))
				{
					pErr->setErrBit(ERR_ADD_PRES_TIME_OUT);
				}
				else
				{
					pErr->setErrBit(ERR_WATER_PRES);
				}
				me->stateID = STATE_WATER_OUT;
				enterWATER_OUT(me);
			}
			break;
		case STATE_LEAK_TEST:
			while(me->its_pSubLeakTestProcessor->getCurrentState(me->its_pSubLeakTestProcessor) != SUB_STATE_READY)
			{
				vTaskDelay(500);
			}
		    exitLEAK_TEST(me);
			me->its_pSubLeakTestProcessor->SendFrame_GetLast(me->its_pSubLeakTestProcessor);
			me->stateID = STATE_WATER_OUT;
			enterWATER_OUT(me);
			break;
		case STATE_WATER_OUT:
			exitWATER_OUT(me);
			me->stateID = STATE_CYLINDER_UP;
			enterCYLINDER_UP(me);
			break;
		case STATE_WATER_PUMP:
			exitWATER_PUMP(me);
			me->stateID = STATE_CYLINDER_UP;
			enterCYLINDER_UP(me);
			break;
		default:
			break;
	}
}

static void TestProcessor_evTimeOut(void)
{
	TestProcessor * me = TestProcessor_create();
	FlagHand * pErr = errHand_create();
	float tempF;
	switch (me->stateID)
	{
		case STATE_CYLINDER_UP:
			if((me->exitSign->getExitSign(me->exitSign, U_CYL_POS_SIGN) == 1) && (me->exitSign->getExitSign(me->exitSign, D_CYL_POS_SIGN) == 0))
			{
				exitCYLINDER_UP(me);
				me->stateID = STATE_CYLINDER_BACK;
				enterCYLINDER_BACK(me);
				pErr->clearErrBit(ERR_CYL_U);
			}
			else
			{
				pErr->setErrBit(ERR_CYL_U);
			}
			break;
		case STATE_CYLINDER_BACK:
			if((me->exitSign->getExitSign(me->exitSign, B_CYL_POS_SIGN) == 1) && (me->exitSign->getExitSign(me->exitSign, F_CYL_POS_SIGN) == 0))
			{
				exitCYLINDER_BACK(me);
				me->stateID = STATE_STOP;
				enterSTOP(me);
				pErr->clearErrBit(ERR_CYL_B);
			}
			else
			{
				pErr->setErrBit(ERR_CYL_B);
			}
			break;
		case STATE_CYLINDER_FORWARD:
			if((me->exitSign->getExitSign(me->exitSign, F_CYL_POS_SIGN) == 1) && (me->exitSign->getExitSign(me->exitSign, B_CYL_POS_SIGN) == 0))
			{
				exitCYLINDER_FORWARD(me);
				me->stateID = STATE_CYLINDER_DOWN;
				enterCYLINDER_DOWN(me);
				pErr->clearErrBit(ERR_CYL_F);
			}
			else
			{
				exitCYLINDER_FORWARD(me);
				me->stateID = STATE_CYLINDER_BACK;
				enterCYLINDER_BACK(me);
				pErr->setErrBit(ERR_CYL_F);
			}
			break;
		case STATE_CYLINDER_DOWN:
			if((me->exitSign->getExitSign(me->exitSign, D_CYL_POS_SIGN) == 1) && (me->exitSign->getExitSign(me->exitSign, U_CYL_POS_SIGN) == 0))
			{
				exitCYLINDER_DOWN(me);
				me->stateID = STATE_13TOR;
				enter13TOR(me);
				pErr->clearErrBit(ERR_CYL_D);
			}
			else
			{
				exitCYLINDER_DOWN(me);
				me->stateID = STATE_CYLINDER_UP;
				enterCYLINDER_UP(me);
				pErr->setErrBit(ERR_CYL_D);
			}
			break;
		case STATE_WATER_IN:
			if(me->exitSign->getExitSign(me->exitSign, WATER_DETEC_SIGN) == 1)
			{
				exitWATER_IN(me);
				me->stateID = STATE_ADD_PRES;
				enterADD_PRES(me);
				pErr->clearErrBit(ERR_WATER_IN_TIME_OUT);
			}
			else
			{
				exitWATER_IN(me);
				me->stateID = STATE_WATER_OUT;
				enterWATER_OUT(me);
				pErr->setErrBit(ERR_WATER_IN_TIME_OUT);
			}
			break;
		case STATE_ADD_PRES:
			tempF = me->adDev->getPresData(me->adDev, AFTER_ADD_WATER);
			if(me->exitSign->getExitSign(me->exitSign, WATER_DETEC_SIGN) && (tempF > me->presPara->highWaterMin) && (tempF < me->presPara->highWaterMax))
			{
				exitADD_PRES(me);
				me->stateID = STATE_LEAK_TEST;
				enterLEAK_TEST(me);
				me->its_pSubLeakTestProcessor->evStart(me->its_pSubLeakTestProcessor);	//泄露测试子流程开始
			}
			else
			{
				exitADD_PRES(me);
				me->stateID = STATE_WATER_OUT;
				enterWATER_OUT(me);
			}
			break;
		case STATE_WATER_OUT:
			exitWATER_OUT(me);
			me->stateID = STATE_WATER_PUMP;
			enterWATER_PUMP(me);
			break;
		default:
			break;			
	}
}

//液位报警信号
//type: 0, 无液位信号； 1， 液位有信号
static void TestProcessor_evLiqSwitch(u8 type)	
{
	TestProcessor * me = TestProcessor_create();
	FlagHand * pErr = errHand_create();
	/*液位开关报警，可以接受液位变化的信号*/
	switch (me->stateID)
	{
		case STATE_WATER_IN:
			/* 注水状态下接收到液位由低变高，属于正常运行 */
			if(type)
			{
				/* 开启倒计时定时器 */
				me->delayTimer->setDelayTime(me->delayTimer, 15); //收到液位信号后继续延时15s,保证液体充满测试腔体
				me->outTimer->stopTimer(me->outTimer); //关闭超时定时器
				pErr->clearErrBit(ERR_LIQ);		//此时是正常运行，清除液位故障
			}
			break;
		case STATE_WATER_OUT:
			/* 此时液位为低， 说明水已经排出，开始倒计时 */
			if(type == 0)
			{
				me->delayTimer->setDelayTime(me->delayTimer, me->timePara->waterDrain); //继续延时一个排水时间，保证水排空管路
				pErr->clearErrBit(ERR_LIQ);		//此时是正常运行，清除液位故障
			}
			break;
		case STATE_ADD_PRES:
		case STATE_LEAK_TEST:
			if(type == 0)
				pErr->setErrBit(ERR_LIQ);
			else
				pErr->clearErrBit(ERR_LIQ);
			break;
		case STATE_STOP:
			//待机状态下 液位由低变高表明液位故障，由高变低清除液位故障
			if(type == 0)
				pErr->clearErrBit(ERR_LIQ);
			break;
		default:
			break;
	}	
}

//气缸上限位传感器
//type: 0,未触发；1，触发
static void TestProcessor_evCylPosU(u8 type)	
{
	TestProcessor * me = TestProcessor_create();
	FlagHand * pErr = errHand_create();
	if((me->stateID == STATE_CYLINDER_UP) && (type == 1))
	{
		exitCYLINDER_UP(me);
		me->stateID = STATE_STOP;
		enterSTOP(me);
		pErr->clearErrBit(ERR_CYL_U);
	}
}

//气缸下限位传感器
//type: 0,未触发；1，触发
static void TestProcessor_evCylPosD(u8 type)	
{
	TestProcessor * me = TestProcessor_create();
	FlagHand * pErr = errHand_create();
	if((me->stateID == STATE_CYLINDER_DOWN) && (type == 1))
	{
		exitCYLINDER_DOWN(me);
		me->stateID = STATE_13TOR;
		enter13TOR(me);
		pErr->clearErrBit(ERR_CYL_D);
	}
}

//气缸前限位传感器
//type: 0,未触发；1，触发
static void TestProcessor_evCylPosF(u8 type)
{
	TestProcessor * me = TestProcessor_create();
	FlagHand * pErr = errHand_create();
	if((me->stateID == STATE_CYLINDER_FORWARD) && (type == 1))
	{
		exitCYLINDER_FORWARD(me);
		me->stateID = STATE_CYLINDER_DOWN;
		enterCYLINDER_DOWN(me);
		pErr->clearErrBit(ERR_CYL_F);
	}
}

//气缸后限位传感器
//type: 0,解除触发；1，触发
static void TestProcessor_evCylPosB(u8 type)
{
	TestProcessor * me = TestProcessor_create();
	FlagHand * pErr = errHand_create();
	if((me->stateID == STATE_CYLINDER_BACK) && (type == 1))
	{
		exitCYLINDER_BACK(me);
		me->stateID = STATE_STOP;
		enterSTOP(me);
		pErr->clearErrBit(ERR_CYL_B);
	}
}

//光栅
//type: 0,解除触发；1，触发
static void TestProcessor_evGrat(u8 type)
{
	TestProcessor * me = TestProcessor_create();
	FlagHand * pErr = errHand_create();
	static u8 uCylLpckState = 0;
	static u8 dCylLpckState = 0;
	static u8 fCylLpckState = 0; 
	static u8 bCylLpckState = 0;
	if(type == 1)
	{
		//收到触发信号，记录当前气缸状态，然后锁定气缸位置并报错
		uCylLpckState = me->valveDev->getValState(me->valveDev, CYL_UP_V);
		dCylLpckState = me->valveDev->getValState(me->valveDev, CYL_DOWN_V);
		fCylLpckState = me->valveDev->getValState(me->valveDev, CYL_FRONT_V);
		bCylLpckState = me->valveDev->getValState(me->valveDev, CYL_BACK_V);
		me->valveDev->setValState(me->valveDev, CYL_UP_V, 0);
		me->valveDev->setValState(me->valveDev, CYL_DOWN_V, 0);
		me->valveDev->setValState(me->valveDev, CYL_FRONT_V, 0);
		me->valveDev->setValState(me->valveDev, CYL_BACK_V, 0);
		pErr->setErrBit(ERR_GRAT);
	}
	else
	{
		//解锁气缸动作，并清除错误
		me->valveDev->setValState(me->valveDev, CYL_UP_V, uCylLpckState);
		me->valveDev->setValState(me->valveDev, CYL_DOWN_V, dCylLpckState);
		me->valveDev->setValState(me->valveDev, CYL_FRONT_V, fCylLpckState);
		me->valveDev->setValState(me->valveDev, CYL_BACK_V, bCylLpckState);
		uCylLpckState = 0;
		dCylLpckState = 0;
		fCylLpckState = 0;
		bCylLpckState = 0;
		pErr->clearErrBit(ERR_GRAT);
	}
}

/*---------------------------------------------状态机动作函数------------------------------------------*/
void enterSTOP(TestProcessor * const me)
{
	int i = 0;
	AppMessage msg;
	static float iniTime = 0.0;
	FlagHand *pErr = errHand_create();	//错误处理对象
	for(i = WATER_IN_V; i < MAX_NUM_V; i++)
	{
		//待机时，大真空、微真空、平衡和气缸下进气控制阀打开，其他阀门关闭
		if((i == CYL_FRONT_V) || (i == CYL_DOWN_V) || (i == BALANCE_V) || (i == BIG_VACUUM_V) || (i == LIT_VACUUM_V))
		{
			me->valveDev->setValState(me->valveDev, i, 1);
		}
		else
		{
			me->valveDev->setValState(me->valveDev, i, 0);
		}
	}
	me->delayTimer->stopTimer(me->delayTimer);	//关闭延时时间更新定时
	me->proporV->setOutputPres(me->proporV, 0);	//设置PWM电压0V
	/* 打开加压阀和排水阀，1s后关闭，排出里面多余的气体 */
	me->valveDev->setValState(me->valveDev, PRES_TO_WATER_V, 1);
	me->valveDev->setValState(me->valveDev, WATER_OUT_V, 1);
	vTaskDelay(1000);
	me->valveDev->setValState(me->valveDev, PRES_TO_WATER_V, 0);
	vTaskDelay(1000);
	me->valveDev->setValState(me->valveDev, WATER_OUT_V, 0);
	/* 给modbus发送消息，更新当前状态，直接发送指针 */
	msg.dataType = MB_UPDATE_STATE;
	msg.pVoid = (void *)MB_STATE_READY;
	xQueueSend(modbusQ, &msg, 10);
	/* 给modbus发送消息，更新倒计时寄存器 */
	msg.dataType = MB_UPDATE_DELAY_TIME;
	msg.pVoid = &iniTime;
	xQueueSend(modbusQ, &msg, 10);	
	
	pErr->setVbStatus(EX_END_S);	//机械手END信号
	printf("enterSTOP\r\n");
}

void enterCYLINDER_FORWARD(TestProcessor * const me)
{
}

void enterCYLINDER_DOWN(TestProcessor * const me)
{
	int i = 0;
	AppMessage msg;
	for(i = 0; i < MAX_NUM_V; i++)
	{
		//气缸下压状态，平衡和气缸上进气控制阀打开，其余关闭
		if((i == BALANCE_V) || (i == CYL_UP_V))
		{
			me->valveDev->setValState(me->valveDev, i, 1);
		}
		else
		{
			me->valveDev->setValState(me->valveDev, i, 0);
		}
	}
	/* 给modbus发送消息，更新当前状态，直接发送指针 */
	msg.dataType = MB_UPDATE_STATE;
	msg.pVoid = (void *)MB_STATE_CYL_DOWN;
	xQueueSend(modbusQ, &msg, 10);
	me->outTimer->setDelayTime(me->outTimer, 30); //启动超时定时器，定时时间30s
	printf("enterCYLINDER_DOWN\r\n");
}

void enter13TOR(TestProcessor * const me)
{
	int i = 0;
	AppMessage msg;
	for(i = 0; i < MAX_NUM_V; i++)
	{
		//针阀、平衡阀、气缸上进气控制阀打开，其余阀门关闭
		if((i == BALANCE_V) || (i == NEEDLE_V) || (i == CYL_UP_V))
		{
			me->valveDev->setValState(me->valveDev, i, 1);
		}
		else
		{
			me->valveDev->setValState(me->valveDev, i, 0);
		}
	}
	/* 给modbus发送消息，更新当前状态，直接发送指针 */
	msg.dataType = MB_UPDATE_STATE;
	msg.pVoid = (void *)MB_STATE_13TOR;
	xQueueSend(modbusQ, &msg, 10);
	me->delayTimer->setDelayTime(me->delayTimer, me->timePara->tor13); //启动延时定时器
	printf("ente13TOR\r\n");
}

void enterPRODUCT_TEST(TestProcessor * const me)
{
	int i = 0;
	AppMessage msg;
	for(i = 0; i < MAX_NUM_V; i++)
	{
		//注水阀、排水阀、抽水阀、气缸上进气控制阀打开，其他阀门关闭
		if((i == WATER_IN_V) || (i == PUMP_WATER_V) || (i == WATER_OUT_V) || (i == BALANCE_V) || (i == CYL_UP_V))
		{
			me->valveDev->setValState(me->valveDev, i, 1);
		}
		else
		{
			me->valveDev->setValState(me->valveDev, i, 0);
		}
	}
	/* 给modbus发送消息，更新当前状态，直接发送指针 */
	msg.dataType = MB_UPDATE_STATE;
	msg.pVoid = (void *)MB_STATE_EXIST;
	xQueueSend(modbusQ, &msg, 10);
	me->delayTimer->setDelayTime(me->delayTimer, me->timePara->productDetc); //启动延时定时器
	printf("enterPRODUCT_TEST\r\n");
}

//进入管路吹气状态
void enterAIR_PURGE(TestProcessor * const me)
{
	int i = 0;
	AppMessage msg;
	for(i = 0; i < MAX_NUM_V; i++)
	{
		//微漏大漏1、2、3、4，干燥阀、排气阀、气缸上进气控制阀打开，其他阀门关闭
		if((i == LIT_LEAK_1_V) || (i == LIT_LEAK_2_V) || (i == LIT_LEAK_3_V) || (i == LIT_LEAK_4_V) || 
		   (i == BIG_LEAK_1_V) || (i == BIG_LEAK_2_V) || (i == BIG_LEAK_3_V) || (i == BIG_LEAK_4_V) ||
		   (i == DRY_V) || (i == AIR_OUT_V) || (i == CYL_UP_V))
		{
			me->valveDev->setValState(me->valveDev, i, 1);
		}
		else
		{
			me->valveDev->setValState(me->valveDev, i, 0);
		}
	}
	/* 给modbus发送消息，更新当前状态，直接发送指针 */
	msg.dataType = MB_UPDATE_STATE;
	msg.pVoid = (void *)MB_STATE_PURGE;
	xQueueSend(modbusQ, &msg, 10);
	me->delayTimer->setDelayTime(me->delayTimer, me->timePara->pipeAirPurge); //启动延时定时器
	printf("enterAIR_PURGE\r\n");
}

void enterAIR_PUMP(TestProcessor * const me)
{
	int i = 0;
	AppMessage msg;
	for(i = 0; i < MAX_NUM_V; i++)
	{
		//气缸上进气控制阀打开，其他阀门关闭
		if((i == BALANCE_V) || (i == CYL_UP_V))
		{
			me->valveDev->setValState(me->valveDev, i, 1);
		}
		else
		{
			me->valveDev->setValState(me->valveDev, i, 0);
		}
	}
	if(xTimerIsTimerActive(me->its_pSubLeakTestProcessor->timerHandle) == pdFALSE)
	{
		xTimerStart(me->its_pSubLeakTestProcessor->timerHandle, 10);
	}
	me->its_pSubLeakTestProcessor->CF28Lpv->StartCycle(me->its_pSubLeakTestProcessor->CF28Lpv, &me->its_pSubLeakTestProcessor->MbTcp_ProtFrame);
	/* 给modbus发送消息，更新当前状态，直接发送指针 */
	msg.dataType = MB_UPDATE_STATE;
	msg.pVoid = (void *)MB_STATE_PUMP;
	xQueueSend(modbusQ, &msg, 10);
	me->delayTimer->setDelayTime(me->delayTimer, me->timePara->pipeAirPump); //启动延时定时器
	printf("enterAIR_PUMP\r\n");
}

void enterIS_DRY(TestProcessor * const me)
{
	int i = 0;
	AppMessage msg;
	for(i = 0; i < MAX_NUM_V; i++)
	{
		//大微真空、平衡阀、气缸上进气控制阀打开，其他阀门关闭
		if((i == BALANCE_V) || (i == CYL_UP_V))
		{
			me->valveDev->setValState(me->valveDev, i, 1);
		}
		else
		{
			me->valveDev->setValState(me->valveDev, i, 0);
		}
	}
	/* 给modbus发送消息，更新当前状态，直接发送指针 */
	msg.dataType = MB_UPDATE_STATE;
	msg.pVoid = (void *)MB_STATE_IS_DRY;
	xQueueSend(modbusQ, &msg, 10);
	me->delayTimer->setDelayTime(me->delayTimer, me->timePara->pipeIsDry); //启动延时定时器
	printf("enterIS_DRY\r\n");
}

void enterWATER_IN(TestProcessor * const me)
{
	int i = 0;
	AppMessage msg;
	for(i = 0; i < MAX_NUM_V; i++)
	{
		//大微真空、平衡阀、微漏1、2、3、4、针阀、注水、排水、水泵、气缸上进气控制阀打开，其他阀门关闭
		if((i == BALANCE_V) || (i == WATER_IN_V) || (i == WATER_OUT_V) || (i == WATER_PUMP) || (i == CYL_UP_V))
		{
			me->valveDev->setValState(me->valveDev, i, 1);
		}
		else
		{
			me->valveDev->setValState(me->valveDev, i, 0);
		}
	}
	/* 给modbus发送消息，更新当前状态，直接发送指针 */
	msg.dataType = MB_UPDATE_STATE;
	msg.pVoid = (void *)MB_STATE_WATER_IN;
	xQueueSend(modbusQ, &msg, 10);
	me->outTimer->setDelayTime(me->outTimer, 25); //启动超时定时器，定时时间25s
	printf("enterWATER_IN\r\n");
}

void enterADD_PRES(TestProcessor * const me)
{
	int i = 0;
	AppMessage msg;
	for(i = 0; i < MAX_NUM_V; i++)
	{
		//平衡阀、针阀、加压、气缸上进气控制阀打开，其他阀门关闭
		if((i == BALANCE_V) || (i == NEEDLE_V) || (i == PRES_TO_WATER_V) || (i == CYL_UP_V))
		{
			me->valveDev->setValState(me->valveDev, i, 1);
		}
		else
		{
			me->valveDev->setValState(me->valveDev, i, 0);
		}
	}
	/* 给modbus发送消息，更新当前状态，直接发送指针 */
	msg.dataType = MB_UPDATE_STATE;
	msg.pVoid = (void *)MB_STATE_ADD_PRES;
	xQueueSend(modbusQ, &msg, 10);
	me->proporV->setOutputPres(me->proporV, me->presPara->highWaterOpening);
	me->delayTimer->setDelayTime(me->delayTimer, me->timePara->addPres); //启动延时定时器
	printf("enterADD_PRES\r\n");
}

void enterLEAK_TEST(TestProcessor * const me)
{
	//此处由子状态进行处理
	me->its_pSubLeakTestProcessor->evInit(me->its_pSubLeakTestProcessor);
	me->delayTimer->setDelayTime(me->delayTimer, 45);	//给予的测试时间位45s
}

//进入排水状态
void enterWATER_OUT(TestProcessor * const me)
{
	int i = 0;
	AppMessage msg;
	for(i = 0; i < MAX_NUM_V; i++)
	{
		//微真空、微漏大漏1、2、3、4，干燥、加压、排气、排水、气缸上进气控制阀打开，其他阀门关闭
		if( (i == PRES_TO_WATER_V) || (i == PUMP_WATER_V) || (i == WATER_OUT_V) || (i == CYL_UP_V))
		{
			me->valveDev->setValState(me->valveDev, i, 1);
		}
		else
		{
			me->valveDev->setValState(me->valveDev, i, 0);
		}
	}
	/* 给modbus发送消息，更新当前状态，直接发送指针 */
	msg.dataType = MB_UPDATE_STATE;
	msg.pVoid = (void *)MB_STATE_WATER_OUT;
	xQueueSend(modbusQ, &msg, 10);
	me->outTimer->setDelayTime(me->outTimer, 20); 
	printf("enterWATER_OUT\r\n");
}

void enterWATER_PUMP(TestProcessor * const me)
{
	int i = 0;
	AppMessage msg;
	for(i = 0; i < MAX_NUM_V; i++)
	{
		//微真空、微漏大漏1、2、3、4，干燥、进气、排气、抽气、抽水、气缸上进气控制阀打开，其他阀门关闭
		if((i == LIT_LEAK_1_V) || (i == LIT_LEAK_2_V) || (i == LIT_LEAK_3_V) || (i == LIT_LEAK_4_V) || 
		   (i == BIG_LEAK_1_V) || (i == BIG_LEAK_2_V) || (i == BIG_LEAK_3_V) || (i == BIG_LEAK_4_V) ||
		   (i == LIT_VACUUM_V) || (i == DRY_V) || (i == AIR_IN_V) || (i == AIR_OUT_V) ||
		   (i == PUMP_WATER_V) || (i == CYL_UP_V))
		{
			me->valveDev->setValState(me->valveDev, i, 1);
		}
		else
		{
			me->valveDev->setValState(me->valveDev, i, 0);
		}
	}
	/* 给modbus发送消息，更新当前状态，直接发送指针 */
	msg.dataType = MB_UPDATE_STATE;
	msg.pVoid = (void *)MB_STATE_WATER_PUMP;
	xQueueSend(modbusQ, &msg, 10);
	me->delayTimer->setDelayTime(me->delayTimer, me->timePara->waterPump); //启动延时定时器
	printf("enterWATER_PUMP\r\n");
}

//进入气缸上抬状态
void enterCYLINDER_UP(TestProcessor * const me)
{
	int i = 0;
	AppMessage msg;
	for(i = 0; i < MAX_NUM_V; i++)
	{
		//微真空、大漏1、2、3、4，干燥、平衡、加压、排气、排水、气缸上进气控制阀打开，其他阀门关闭
		if(i == CYL_DOWN_V)
		{
			me->valveDev->setValState(me->valveDev, i, 1);
		}
		else
		{
			me->valveDev->setValState(me->valveDev, i, 0);
		}
	}
	/* 给modbus发送消息，更新当前状态，直接发送指针 */
	msg.dataType = MB_UPDATE_STATE;
	msg.pVoid = (void *)MB_STATE_CYL_UP;
	xQueueSend(modbusQ, &msg, 10);
	me->outTimer->setDelayTime(me->outTimer, 30); 
	printf("enterCYLINDER_UP\r\n");
}

void enterCYLINDER_BACK(TestProcessor * const me)
{
}

//离开待机状态
void exitSTOP(TestProcessor * const me)
{
}


void exitCYLINDER_FORWARD(TestProcessor * const me)
{
}

//离开气缸下压状态
void exitCYLINDER_DOWN(TestProcessor * const me)
{
	me->outTimer->stopTimer(me->outTimer);
}

void exit13TOR(TestProcessor * const me)
{
	me->delayTimer->stopTimer(me->delayTimer);
}

void exitPRODUCT_TEST(TestProcessor * const me)
{
	me->delayTimer->stopTimer(me->delayTimer);
}

void exitAIR_PURGE(TestProcessor * const me)
{
	me->delayTimer->stopTimer(me->delayTimer);
}

void exitAIR_PUMP(TestProcessor * const me)
{
	if(xTimerIsTimerActive(me->its_pSubLeakTestProcessor->timerHandle) != pdFALSE)
	{
		xTimerStop(me->its_pSubLeakTestProcessor->timerHandle, 10);
	}
	me->its_pSubLeakTestProcessor->CF28Lpv->StopCycle(me->its_pSubLeakTestProcessor->CF28Lpv, &me->its_pSubLeakTestProcessor->MbTcp_ProtFrame);
	me->delayTimer->stopTimer(me->delayTimer);
}

void exitIS_DRY(TestProcessor * const me)
{
	me->delayTimer->stopTimer(me->delayTimer);
}

void exitWATER_IN(TestProcessor * const me)
{
	me->delayTimer->stopTimer(me->delayTimer);
	me->outTimer->stopTimer(me->outTimer);
	//由于阀门开关顺序可能会导致腔内气压升高，因此做一定的延时处理
	me->valveDev->setValState(me->valveDev, WATER_IN_V, 0); //关闭注水阀
	me->valveDev->setValState(me->valveDev, WATER_PUMP, 0);	//关闭水泵
	vTaskDelay(500); //延时500ms
	me->valveDev->setValState(me->valveDev, WATER_OUT_V, 0); //关闭排水阀
	vTaskDelay(200);
	me->valveDev->setValState(me->valveDev, PRES_TO_WATER_V, 1); //打开加压阀
	
	//设定比例阀为20Kpa, 给modbus发消息清零数据更新比例阀开度
	me->proporV->setOutputPres(me->proporV, me->presPara->lowWaterOpening);

	printf("exitWATER_IN\r\n");
	
}

void exitADD_PRES(TestProcessor * const me)
{
	me->delayTimer->stopTimer(me->delayTimer);
}

void exitLEAK_TEST(TestProcessor * const me)
{
	if(xTimerIsTimerActive(me->its_pSubLeakTestProcessor->timerHandle) != pdFALSE)
	{
		xTimerStop(me->its_pSubLeakTestProcessor->timerHandle, 10);
	}
	me->delayTimer->stopTimer(me->delayTimer);
}

void exitWATER_OUT(TestProcessor * const me)
{
	me->delayTimer->stopTimer(me->delayTimer);
	me->outTimer->stopTimer(me->outTimer);
	
	//排水结束后，要关掉电子比例阀，并延时一段时间，以便腔内剩余气体排出
	me->proporV->setOutputPres(me->proporV, 0); //关闭比例阀
	vTaskDelay(500);
	me->valveDev->setValState(me->valveDev, PRES_TO_WATER_V, 0); //关闭加压阀	
	vTaskDelay(200);
	printf("exitWATER_OUT\r\n");
}

void exitWATER_PUMP(TestProcessor * const me)
{
	me->delayTimer->stopTimer(me->delayTimer);
}

void exitCYLINDER_UP(TestProcessor * const me)
{
	me->outTimer->stopTimer(me->outTimer);
}

void exitCYLINDER_BACK(TestProcessor * const me)
{
}
