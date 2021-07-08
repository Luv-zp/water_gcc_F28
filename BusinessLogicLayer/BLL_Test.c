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
static void TestProcessor_evTimeOut(void);		//��ʱ
static void TestProcessor_evLiqSwitch(u8 type);	//Һλ����
static void TestProcessor_evCylPosU(u8 type);	//��������λ������
static void TestProcessor_evCylPosD(u8 type);	//��������λ������
static void TestProcessor_evCylPosF(u8 type);	//����ǰ��λ������
static void TestProcessor_evCylPosB(u8 type);	//���׺���λ������
static void TestProcessor_evGrat(u8 type);	//��դ

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




static TestProcessor testProcessor;		//���Ի�(��̬)

//����ͬ��״̬������
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
		pErr->clearAllVbRes();	//����ϴβ��Խ��
		if(me->exitSign->getExitSign(me->exitSign, GRAT_SIGN))	//����դ�Ƿ�����
			pErr->setErrBit(ERR_GRAT);	//����
		else
			pErr->clearErrBit(ERR_GRAT);	//���ͨ�����������
		if(me->exitSign->getExitSign(me->exitSign, VACUUM_SIGN))	//�����մ������Ƿ�����
			pErr->setErrBit(ERR_VACUUM);	//����
		else
			pErr->clearErrBit(ERR_VACUUM);	//���ͨ�����������
		if(me->exitSign->getExitSign(me->exitSign, WATER_DETEC_SIGN))	//���Һλ�������Ƿ�����
			pErr->setErrBit(ERR_LIQ);
		else
			pErr->clearErrBit(ERR_LIQ);
		if(me->exitSign->getExitSign(me->exitSign, U_CYL_POS_SIGN == 0))	//������״��Կ����Ƿ�����
			pErr->setErrBit(ERR_CYL_U);
		else
			pErr->clearErrBit(ERR_CYL_U);
		if(me->exitSign->getExitSign(me->exitSign, D_CYL_POS_SIGN == 1))
			pErr->setErrBit(ERR_CYL_D);
		else
			pErr->clearErrBit(ERR_CYL_D);
		if(pErr->getAllErr()) return; 	//��⵽�д���ֱ���˳�		
		
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
			/* ����״̬������ֹͣ��Ϣ��ֻ�е���״̬��ֹͣ��Ż�������״̬ */
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
			/* ��Ʒ��⵹��ʱ��������������ˣ���ʱ����ˮѹ�������������жϲ�Ʒ���ޣ��������������� */
			tempF = me->adDev->getPresData(me->adDev, PRO_DEC_WATER);
			exitPRODUCT_TEST(me);
			/*����ʵ�ʲ��Է��֣�������©������Ʒ���ʱˮѹԼΪ-50KPa, ������©�����ʱԼΪ-19KPa*/
			if((tempF < 0) && (me->presPara->existLimit < fabs(tempF)))
			{
				//��Ʒ���ڣ���������
				me->stateID = STATE_AIR_PUMP;
				enterAIR_PUMP(me);	//�������
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
				me->its_pSubLeakTestProcessor->evStart(me->its_pSubLeakTestProcessor);	//й¶���������̿�ʼ
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
				me->its_pSubLeakTestProcessor->evStart(me->its_pSubLeakTestProcessor);	//й¶���������̿�ʼ
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

//Һλ�����ź�
//type: 0, ��Һλ�źţ� 1�� Һλ���ź�
static void TestProcessor_evLiqSwitch(u8 type)	
{
	TestProcessor * me = TestProcessor_create();
	FlagHand * pErr = errHand_create();
	/*Һλ���ر��������Խ���Һλ�仯���ź�*/
	switch (me->stateID)
	{
		case STATE_WATER_IN:
			/* עˮ״̬�½��յ�Һλ�ɵͱ�ߣ������������� */
			if(type)
			{
				/* ��������ʱ��ʱ�� */
				me->delayTimer->setDelayTime(me->delayTimer, 15); //�յ�Һλ�źź������ʱ15s,��֤Һ���������ǻ��
				me->outTimer->stopTimer(me->outTimer); //�رճ�ʱ��ʱ��
				pErr->clearErrBit(ERR_LIQ);		//��ʱ���������У����Һλ����
			}
			break;
		case STATE_WATER_OUT:
			/* ��ʱҺλΪ�ͣ� ˵��ˮ�Ѿ��ų�����ʼ����ʱ */
			if(type == 0)
			{
				me->delayTimer->setDelayTime(me->delayTimer, me->timePara->waterDrain); //������ʱһ����ˮʱ�䣬��֤ˮ�ſչ�·
				pErr->clearErrBit(ERR_LIQ);		//��ʱ���������У����Һλ����
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
			//����״̬�� Һλ�ɵͱ�߱���Һλ���ϣ��ɸ߱�����Һλ����
			if(type == 0)
				pErr->clearErrBit(ERR_LIQ);
			break;
		default:
			break;
	}	
}

//��������λ������
//type: 0,δ������1������
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

//��������λ������
//type: 0,δ������1������
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

//����ǰ��λ������
//type: 0,δ������1������
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

//���׺���λ������
//type: 0,���������1������
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

//��դ
//type: 0,���������1������
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
		//�յ������źţ���¼��ǰ����״̬��Ȼ����������λ�ò�����
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
		//�������׶��������������
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

/*---------------------------------------------״̬����������------------------------------------------*/
void enterSTOP(TestProcessor * const me)
{
	int i = 0;
	AppMessage msg;
	static float iniTime = 0.0;
	FlagHand *pErr = errHand_create();	//���������
	for(i = WATER_IN_V; i < MAX_NUM_V; i++)
	{
		//����ʱ������ա�΢��ա�ƽ��������½������Ʒ��򿪣��������Źر�
		if((i == CYL_FRONT_V) || (i == CYL_DOWN_V) || (i == BALANCE_V) || (i == BIG_VACUUM_V) || (i == LIT_VACUUM_V))
		{
			me->valveDev->setValState(me->valveDev, i, 1);
		}
		else
		{
			me->valveDev->setValState(me->valveDev, i, 0);
		}
	}
	me->delayTimer->stopTimer(me->delayTimer);	//�ر���ʱʱ����¶�ʱ
	me->proporV->setOutputPres(me->proporV, 0);	//����PWM��ѹ0V
	/* �򿪼�ѹ������ˮ����1s��رգ��ų������������� */
	me->valveDev->setValState(me->valveDev, PRES_TO_WATER_V, 1);
	me->valveDev->setValState(me->valveDev, WATER_OUT_V, 1);
	vTaskDelay(1000);
	me->valveDev->setValState(me->valveDev, PRES_TO_WATER_V, 0);
	vTaskDelay(1000);
	me->valveDev->setValState(me->valveDev, WATER_OUT_V, 0);
	/* ��modbus������Ϣ�����µ�ǰ״̬��ֱ�ӷ���ָ�� */
	msg.dataType = MB_UPDATE_STATE;
	msg.pVoid = (void *)MB_STATE_READY;
	xQueueSend(modbusQ, &msg, 10);
	/* ��modbus������Ϣ�����µ���ʱ�Ĵ��� */
	msg.dataType = MB_UPDATE_DELAY_TIME;
	msg.pVoid = &iniTime;
	xQueueSend(modbusQ, &msg, 10);	
	
	pErr->setVbStatus(EX_END_S);	//��е��END�ź�
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
		//������ѹ״̬��ƽ��������Ͻ������Ʒ��򿪣�����ر�
		if((i == BALANCE_V) || (i == CYL_UP_V))
		{
			me->valveDev->setValState(me->valveDev, i, 1);
		}
		else
		{
			me->valveDev->setValState(me->valveDev, i, 0);
		}
	}
	/* ��modbus������Ϣ�����µ�ǰ״̬��ֱ�ӷ���ָ�� */
	msg.dataType = MB_UPDATE_STATE;
	msg.pVoid = (void *)MB_STATE_CYL_DOWN;
	xQueueSend(modbusQ, &msg, 10);
	me->outTimer->setDelayTime(me->outTimer, 30); //������ʱ��ʱ������ʱʱ��30s
	printf("enterCYLINDER_DOWN\r\n");
}

void enter13TOR(TestProcessor * const me)
{
	int i = 0;
	AppMessage msg;
	for(i = 0; i < MAX_NUM_V; i++)
	{
		//�뷧��ƽ�ⷧ�������Ͻ������Ʒ��򿪣����෧�Źر�
		if((i == BALANCE_V) || (i == NEEDLE_V) || (i == CYL_UP_V))
		{
			me->valveDev->setValState(me->valveDev, i, 1);
		}
		else
		{
			me->valveDev->setValState(me->valveDev, i, 0);
		}
	}
	/* ��modbus������Ϣ�����µ�ǰ״̬��ֱ�ӷ���ָ�� */
	msg.dataType = MB_UPDATE_STATE;
	msg.pVoid = (void *)MB_STATE_13TOR;
	xQueueSend(modbusQ, &msg, 10);
	me->delayTimer->setDelayTime(me->delayTimer, me->timePara->tor13); //������ʱ��ʱ��
	printf("ente13TOR\r\n");
}

void enterPRODUCT_TEST(TestProcessor * const me)
{
	int i = 0;
	AppMessage msg;
	for(i = 0; i < MAX_NUM_V; i++)
	{
		//עˮ������ˮ������ˮ���������Ͻ������Ʒ��򿪣��������Źر�
		if((i == WATER_IN_V) || (i == PUMP_WATER_V) || (i == WATER_OUT_V) || (i == BALANCE_V) || (i == CYL_UP_V))
		{
			me->valveDev->setValState(me->valveDev, i, 1);
		}
		else
		{
			me->valveDev->setValState(me->valveDev, i, 0);
		}
	}
	/* ��modbus������Ϣ�����µ�ǰ״̬��ֱ�ӷ���ָ�� */
	msg.dataType = MB_UPDATE_STATE;
	msg.pVoid = (void *)MB_STATE_EXIST;
	xQueueSend(modbusQ, &msg, 10);
	me->delayTimer->setDelayTime(me->delayTimer, me->timePara->productDetc); //������ʱ��ʱ��
	printf("enterPRODUCT_TEST\r\n");
}

//�����·����״̬
void enterAIR_PURGE(TestProcessor * const me)
{
	int i = 0;
	AppMessage msg;
	for(i = 0; i < MAX_NUM_V; i++)
	{
		//΢©��©1��2��3��4�����﷧���������������Ͻ������Ʒ��򿪣��������Źر�
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
	/* ��modbus������Ϣ�����µ�ǰ״̬��ֱ�ӷ���ָ�� */
	msg.dataType = MB_UPDATE_STATE;
	msg.pVoid = (void *)MB_STATE_PURGE;
	xQueueSend(modbusQ, &msg, 10);
	me->delayTimer->setDelayTime(me->delayTimer, me->timePara->pipeAirPurge); //������ʱ��ʱ��
	printf("enterAIR_PURGE\r\n");
}

void enterAIR_PUMP(TestProcessor * const me)
{
	int i = 0;
	AppMessage msg;
	for(i = 0; i < MAX_NUM_V; i++)
	{
		//�����Ͻ������Ʒ��򿪣��������Źر�
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
	/* ��modbus������Ϣ�����µ�ǰ״̬��ֱ�ӷ���ָ�� */
	msg.dataType = MB_UPDATE_STATE;
	msg.pVoid = (void *)MB_STATE_PUMP;
	xQueueSend(modbusQ, &msg, 10);
	me->delayTimer->setDelayTime(me->delayTimer, me->timePara->pipeAirPump); //������ʱ��ʱ��
	printf("enterAIR_PUMP\r\n");
}

void enterIS_DRY(TestProcessor * const me)
{
	int i = 0;
	AppMessage msg;
	for(i = 0; i < MAX_NUM_V; i++)
	{
		//��΢��ա�ƽ�ⷧ�������Ͻ������Ʒ��򿪣��������Źر�
		if((i == BALANCE_V) || (i == CYL_UP_V))
		{
			me->valveDev->setValState(me->valveDev, i, 1);
		}
		else
		{
			me->valveDev->setValState(me->valveDev, i, 0);
		}
	}
	/* ��modbus������Ϣ�����µ�ǰ״̬��ֱ�ӷ���ָ�� */
	msg.dataType = MB_UPDATE_STATE;
	msg.pVoid = (void *)MB_STATE_IS_DRY;
	xQueueSend(modbusQ, &msg, 10);
	me->delayTimer->setDelayTime(me->delayTimer, me->timePara->pipeIsDry); //������ʱ��ʱ��
	printf("enterIS_DRY\r\n");
}

void enterWATER_IN(TestProcessor * const me)
{
	int i = 0;
	AppMessage msg;
	for(i = 0; i < MAX_NUM_V; i++)
	{
		//��΢��ա�ƽ�ⷧ��΢©1��2��3��4���뷧��עˮ����ˮ��ˮ�á������Ͻ������Ʒ��򿪣��������Źر�
		if((i == BALANCE_V) || (i == WATER_IN_V) || (i == WATER_OUT_V) || (i == WATER_PUMP) || (i == CYL_UP_V))
		{
			me->valveDev->setValState(me->valveDev, i, 1);
		}
		else
		{
			me->valveDev->setValState(me->valveDev, i, 0);
		}
	}
	/* ��modbus������Ϣ�����µ�ǰ״̬��ֱ�ӷ���ָ�� */
	msg.dataType = MB_UPDATE_STATE;
	msg.pVoid = (void *)MB_STATE_WATER_IN;
	xQueueSend(modbusQ, &msg, 10);
	me->outTimer->setDelayTime(me->outTimer, 25); //������ʱ��ʱ������ʱʱ��25s
	printf("enterWATER_IN\r\n");
}

void enterADD_PRES(TestProcessor * const me)
{
	int i = 0;
	AppMessage msg;
	for(i = 0; i < MAX_NUM_V; i++)
	{
		//ƽ�ⷧ���뷧����ѹ�������Ͻ������Ʒ��򿪣��������Źر�
		if((i == BALANCE_V) || (i == NEEDLE_V) || (i == PRES_TO_WATER_V) || (i == CYL_UP_V))
		{
			me->valveDev->setValState(me->valveDev, i, 1);
		}
		else
		{
			me->valveDev->setValState(me->valveDev, i, 0);
		}
	}
	/* ��modbus������Ϣ�����µ�ǰ״̬��ֱ�ӷ���ָ�� */
	msg.dataType = MB_UPDATE_STATE;
	msg.pVoid = (void *)MB_STATE_ADD_PRES;
	xQueueSend(modbusQ, &msg, 10);
	me->proporV->setOutputPres(me->proporV, me->presPara->highWaterOpening);
	me->delayTimer->setDelayTime(me->delayTimer, me->timePara->addPres); //������ʱ��ʱ��
	printf("enterADD_PRES\r\n");
}

void enterLEAK_TEST(TestProcessor * const me)
{
	//�˴�����״̬���д���
	me->its_pSubLeakTestProcessor->evInit(me->its_pSubLeakTestProcessor);
	me->delayTimer->setDelayTime(me->delayTimer, 45);	//����Ĳ���ʱ��λ45s
}

//������ˮ״̬
void enterWATER_OUT(TestProcessor * const me)
{
	int i = 0;
	AppMessage msg;
	for(i = 0; i < MAX_NUM_V; i++)
	{
		//΢��ա�΢©��©1��2��3��4�������ѹ����������ˮ�������Ͻ������Ʒ��򿪣��������Źر�
		if( (i == PRES_TO_WATER_V) || (i == PUMP_WATER_V) || (i == WATER_OUT_V) || (i == CYL_UP_V))
		{
			me->valveDev->setValState(me->valveDev, i, 1);
		}
		else
		{
			me->valveDev->setValState(me->valveDev, i, 0);
		}
	}
	/* ��modbus������Ϣ�����µ�ǰ״̬��ֱ�ӷ���ָ�� */
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
		//΢��ա�΢©��©1��2��3��4�������������������������ˮ�������Ͻ������Ʒ��򿪣��������Źر�
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
	/* ��modbus������Ϣ�����µ�ǰ״̬��ֱ�ӷ���ָ�� */
	msg.dataType = MB_UPDATE_STATE;
	msg.pVoid = (void *)MB_STATE_WATER_PUMP;
	xQueueSend(modbusQ, &msg, 10);
	me->delayTimer->setDelayTime(me->delayTimer, me->timePara->waterPump); //������ʱ��ʱ��
	printf("enterWATER_PUMP\r\n");
}

//����������̧״̬
void enterCYLINDER_UP(TestProcessor * const me)
{
	int i = 0;
	AppMessage msg;
	for(i = 0; i < MAX_NUM_V; i++)
	{
		//΢��ա���©1��2��3��4�����ƽ�⡢��ѹ����������ˮ�������Ͻ������Ʒ��򿪣��������Źر�
		if(i == CYL_DOWN_V)
		{
			me->valveDev->setValState(me->valveDev, i, 1);
		}
		else
		{
			me->valveDev->setValState(me->valveDev, i, 0);
		}
	}
	/* ��modbus������Ϣ�����µ�ǰ״̬��ֱ�ӷ���ָ�� */
	msg.dataType = MB_UPDATE_STATE;
	msg.pVoid = (void *)MB_STATE_CYL_UP;
	xQueueSend(modbusQ, &msg, 10);
	me->outTimer->setDelayTime(me->outTimer, 30); 
	printf("enterCYLINDER_UP\r\n");
}

void enterCYLINDER_BACK(TestProcessor * const me)
{
}

//�뿪����״̬
void exitSTOP(TestProcessor * const me)
{
}


void exitCYLINDER_FORWARD(TestProcessor * const me)
{
}

//�뿪������ѹ״̬
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
	//���ڷ��ſ���˳����ܻᵼ��ǻ����ѹ���ߣ������һ������ʱ����
	me->valveDev->setValState(me->valveDev, WATER_IN_V, 0); //�ر�עˮ��
	me->valveDev->setValState(me->valveDev, WATER_PUMP, 0);	//�ر�ˮ��
	vTaskDelay(500); //��ʱ500ms
	me->valveDev->setValState(me->valveDev, WATER_OUT_V, 0); //�ر���ˮ��
	vTaskDelay(200);
	me->valveDev->setValState(me->valveDev, PRES_TO_WATER_V, 1); //�򿪼�ѹ��
	
	//�趨������Ϊ20Kpa, ��modbus����Ϣ�������ݸ��±���������
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
	
	//��ˮ������Ҫ�ص����ӱ�����������ʱһ��ʱ�䣬�Ա�ǻ��ʣ�������ų�
	me->proporV->setOutputPres(me->proporV, 0); //�رձ�����
	vTaskDelay(500);
	me->valveDev->setValState(me->valveDev, PRES_TO_WATER_V, 0); //�رռ�ѹ��	
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
