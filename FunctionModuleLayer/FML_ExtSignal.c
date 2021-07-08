#include "FML_ExtSignal.h"


#define EXT_SIGN_DETECT_MS	10

#define WATER_DETECT_TRIG_MS	100
#define WATER_DETECT_UNTRIG_MS	500
#define VACUUM_TRIG_MS		200
#define VACUUM_UNTRIG_MS	50
#define AIR_PRES_TRIG_MS	100
#define AIR_PRES_UNTRIG_MS	100
#define UP_CYL_POS_TRIG_MS		100
#define UP_CYL_POS_UNTRIG_MS	50
#define DOWN_CYL_POS_TRIG_MS	100
#define DOWN_CYL_POS_UNTRIG_MS	50
#define FRONT_CYL_POS_TRIG_MS		100
#define FEONT_CYL_POS_UNTRIG_MS		50
#define BACK_CYL_POS_TRIG_MS	100
#define BACK_CYL_POS_UNTRIG_MS	50
#define GRAT_TRIG_MS	20
#define GRAT_UNTRIG_MS	50

#define LIQ_TRIG			(u32)(WATER_DETECT_TRIG_MS / EXT_SIGN_DETECT_MS)
#define LIQ_UNTRIG			(u32)(WATER_DETECT_UNTRIG_MS / EXT_SIGN_DETECT_MS)
#define VACUUM_TRIG			(u32)(VACUUM_TRIG_MS / EXT_SIGN_DETECT_MS)
#define VACUUM_UNTRIG		(u32)(VACUUM_UNTRIG_MS / EXT_SIGN_DETECT_MS)
#define AIR_TRIG			(u32)(AIR_PRES_TRIG_MS / EXT_SIGN_DETECT_MS)
#define AIR_UNTRIG			(u32)(AIR_PRES_UNTRIG_MS / EXT_SIGN_DETECT_MS)
#define UP_POS_TRIG			(u32)(UP_CYL_POS_TRIG_MS / EXT_SIGN_DETECT_MS)
#define UP_POS_UNTRIG		(u32)(UP_CYL_POS_UNTRIG_MS / EXT_SIGN_DETECT_MS)
#define DOWN_POS_TRIG		(u32)(DOWN_CYL_POS_TRIG_MS / EXT_SIGN_DETECT_MS)
#define DOWN_POS_UNTRIG		(u32)(DOWN_CYL_POS_UNTRIG_MS / EXT_SIGN_DETECT_MS)
#define FRONT_POS_TRIG		(u32)(FRONT_CYL_POS_TRIG_MS / EXT_SIGN_DETECT_MS)
#define FRONT_POS_UNTRIG	(u32)(FEONT_CYL_POS_UNTRIG_MS / EXT_SIGN_DETECT_MS)
#define BACK_POS_TRIG		(u32)(BACK_CYL_POS_TRIG_MS / EXT_SIGN_DETECT_MS)
#define BACK_POS_UNTRIG		(u32)(BACK_CYL_POS_UNTRIG_MS / EXT_SIGN_DETECT_MS)
#define GRAT_TRIG			(u32)(GRAT_TRIG_MS / EXT_SIGN_DETECT_MS)
#define GRAT_UNTRIG			(u32)(GRAT_UNTRIG_MS / EXT_SIGN_DETECT_MS)

static void ExitSign_init(ExitSignal * me);
static void ExitSign_signDetect(ExitSignal * me);
static u8 ExitSign_getExitSign(ExitSignal * me, ExitSign_t type);
static void ExitSign_clearFlag(ExitSignal * me, ExitSign_t type);
static void ExitSign_registerCallback(ExitSignal * me, void * pInshatnce, pGeneralCb p, ExtSigCb_t type);

static void exitSwitchCb(TimerHandle_t xTimer);

static ExitSignal exitSignal;
static NotifyHandle ExitSignal_cbHandle[EXT_CB_MAX];
static TimerHandle_t ES_timerHandle;
static u8 exitSignState = 0;


ExitSignal * ExitSignal_create(void)
{
	static ExitSignal * me = 0;
	int i = 0;
	if(me == 0)
	{
		me = &exitSignal;
		me->init = ExitSign_init;
		me->signDetect = ExitSign_signDetect;
		me->getExitSign = ExitSign_getExitSign;
		me->clearFlag = ExitSign_clearFlag;
		me->registerCallback = ExitSign_registerCallback;
		for(i = 0; i < EXT_CB_MAX; i++)
		{
			me->cbHandle[i] = &ExitSignal_cbHandle[i];
		}
		//�������ش���������
		for(i = 0; i < SIGN_NUM_MAX; i++)
		{
			me->switchSensorArr[i] = SwitchSensor_create(i);
		}	
		
		me->changeFlag = 0;
		me->init(me);	
	}
	return me;
}

static void ExitSign_init(ExitSignal * me)
{
	me->switchSensorArr[WATER_DETEC_SIGN]->setTrigType(me->switchSensorArr[WATER_DETEC_SIGN], HIGH_LEVEL_TRIG);
	me->switchSensorArr[WATER_DETEC_SIGN]->setTrigCountLimit(me->switchSensorArr[WATER_DETEC_SIGN], LIQ_TRIG, LIQ_UNTRIG);
	
	me->switchSensorArr[VACUUM_SIGN]->setTrigType(me->switchSensorArr[VACUUM_SIGN], HIGH_LEVEL_TRIG);
	me->switchSensorArr[VACUUM_SIGN]->setTrigCountLimit(me->switchSensorArr[VACUUM_SIGN], VACUUM_TRIG, VACUUM_UNTRIG);
	
	me->switchSensorArr[AIR_PRES_SIGN]->setTrigType(me->switchSensorArr[AIR_PRES_SIGN], HIGH_LEVEL_TRIG);
	me->switchSensorArr[AIR_PRES_SIGN]->setTrigCountLimit(me->switchSensorArr[AIR_PRES_SIGN], AIR_TRIG, AIR_UNTRIG);
	
	me->switchSensorArr[U_CYL_POS_SIGN]->setTrigType(me->switchSensorArr[U_CYL_POS_SIGN], HIGH_LEVEL_TRIG);
	me->switchSensorArr[U_CYL_POS_SIGN]->setTrigCountLimit(me->switchSensorArr[U_CYL_POS_SIGN], UP_POS_TRIG, UP_POS_UNTRIG);
	
	me->switchSensorArr[D_CYL_POS_SIGN]->setTrigType(me->switchSensorArr[D_CYL_POS_SIGN], HIGH_LEVEL_TRIG);
	me->switchSensorArr[D_CYL_POS_SIGN]->setTrigCountLimit(me->switchSensorArr[D_CYL_POS_SIGN], DOWN_POS_TRIG, DOWN_POS_UNTRIG);
	
	me->switchSensorArr[F_CYL_POS_SIGN]->setTrigType(me->switchSensorArr[F_CYL_POS_SIGN], HIGH_LEVEL_TRIG);
	me->switchSensorArr[F_CYL_POS_SIGN]->setTrigCountLimit(me->switchSensorArr[F_CYL_POS_SIGN], FRONT_POS_TRIG, FRONT_POS_UNTRIG);
	
	me->switchSensorArr[B_CYL_POS_SIGN]->setTrigType(me->switchSensorArr[B_CYL_POS_SIGN], HIGH_LEVEL_TRIG);
	me->switchSensorArr[B_CYL_POS_SIGN]->setTrigCountLimit(me->switchSensorArr[B_CYL_POS_SIGN], BACK_POS_TRIG, BACK_POS_UNTRIG);
	
	me->switchSensorArr[GRAT_SIGN]->setTrigType(me->switchSensorArr[GRAT_SIGN], LOW_LEVEL_TRIG);
	me->switchSensorArr[GRAT_SIGN]->setTrigCountLimit(me->switchSensorArr[GRAT_SIGN], GRAT_TRIG, GRAT_UNTRIG);
	
	ES_timerHandle = xTimerCreate((const char *)"extSign",
								 (TickType_t   )EXT_SIGN_DETECT_MS,
								 (UBaseType_t  )pdTRUE,
								 (void *	   )me,
								 (TimerCallbackFunction_t)exitSwitchCb);	
		
	/*�״μ���ⲿ�źţ���Ҫ���ⲿ�ź�״̬���г�ʼ��*/							 
	if(me->getExitSign(me, WATER_DETEC_SIGN))	exitSignState |= (1 << WATER_DETEC_SIGN);
	if(me->getExitSign(me, VACUUM_SIGN)) 		exitSignState |= (1 << VACUUM_SIGN);
	if(me->getExitSign(me, U_CYL_POS_SIGN))		exitSignState |= (1 << U_CYL_POS_SIGN);
	if(me->getExitSign(me, D_CYL_POS_SIGN)) 	exitSignState |= (1 << D_CYL_POS_SIGN);
	if(me->getExitSign(me, F_CYL_POS_SIGN)) 	exitSignState |= (1 << F_CYL_POS_SIGN);
	if(me->getExitSign(me, B_CYL_POS_SIGN)) 	exitSignState |= (1 << B_CYL_POS_SIGN);
	if(me->getExitSign(me, GRAT_SIGN)) 			exitSignState |= (1 << GRAT_SIGN);
	
	//�����Ӧ�ı�־λ
	me->clearFlag(me, WATER_DETEC_SIGN);
	me->clearFlag(me, VACUUM_SIGN);
	me->clearFlag(me, U_CYL_POS_SIGN);
	me->clearFlag(me, D_CYL_POS_SIGN);
	me->clearFlag(me, F_CYL_POS_SIGN);
	me->clearFlag(me, B_CYL_POS_SIGN);								 
	me->clearFlag(me, GRAT_SIGN);
	
	//�����ź�״̬							 
	if(me->cbHandle[EXT_SIGN_UPDATE]->callback)
	{
		me->cbHandle[EXT_SIGN_UPDATE]->callback(me->cbHandle[EXT_SIGN_UPDATE]->pInstance, &exitSignState);
	}
	
	xTimerStart(ES_timerHandle, 0);						 
}

//�������źţ����б仯���򽫶�Ӧ�ı�־λ��λ
static void ExitSign_signDetect(ExitSignal * me)
{
	int i = 0;
	for(i = 0; i < SIGN_NUM_MAX; i++)
	{
		if(me->switchSensorArr[i]->signDetect(me->switchSensorArr[i], &me->signStateArr[i]))
		{
			me->changeFlag |= (1 << i);
		}
	}
}

//�����ź�״̬
static u8 ExitSign_getExitSign(ExitSignal * me, ExitSign_t type)
{
	return me->signStateArr[type];
}

//���ָ���ı�־λ
static void ExitSign_clearFlag(ExitSignal * me, ExitSign_t type)
{
	me->changeFlag &= ~(1 << type);
}

//��������λ
static void ExitSign_setMask(ExitSign_t type)
{
	exitSignal.maskBit = 1 << type;
}

static void exitSwitchCb(TimerHandle_t xTimer)
{
	ExitSignal * me = ExitSignal_create();
	u8 signState = 0;
	AppMessage msg;
	me->signDetect(me);	//�������п��ش�����
	//��⵽Һλ�б仯
	if(me->changeFlag & 0x01)
	{
		signState = me->getExitSign(me, WATER_DETEC_SIGN);
		if(signState)
		{			
			exitSignState |= (1 << WATER_DETEC_SIGN);
		}
		else
		{
			exitSignState &= ~(1 << WATER_DETEC_SIGN);
		}
		msg.dataType = WATER_DETEC_SIGN;
//		msg.pVoid = (void *)signState; //ֱ�Ӵ���ָ��ֵ �Ͳ����˶���Ϊstatic 
		msg.pVoid = &signState; 
		
		if(me->cbHandle[EXT_SIGN_CHANGE]->callback)
		{
			me->cbHandle[EXT_SIGN_CHANGE]->callback(me->cbHandle[EXT_SIGN_CHANGE]->pInstance, &msg);
		}
		me->clearFlag(me, WATER_DETEC_SIGN);
	}
	//��⵽��մ������б仯
	if(me->changeFlag & 0x02)
	{
		signState = me->getExitSign(me, VACUUM_SIGN);
		if(signState)
		{			
			exitSignState |= (1 << VACUUM_SIGN);
		}
		else
		{
			exitSignState &= ~(1 << VACUUM_SIGN);
		}
		msg.dataType = VACUUM_SIGN;
//		msg.pVoid = (void *)signState; //ֱ�Ӵ���ָ��ֵ;
		msg.pVoid = &signState;
		
		if(me->cbHandle[EXT_SIGN_CHANGE]->callback)
		{
			me->cbHandle[EXT_SIGN_CHANGE]->callback(me->cbHandle[EXT_SIGN_CHANGE]->pInstance, &msg);
		}
		me->clearFlag(me, VACUUM_SIGN);
	}
	//��⵽��������λ�����б仯
	if(me->changeFlag & 0x08)
	{
		signState = me->getExitSign(me, U_CYL_POS_SIGN);
		if(signState)
		{			
			exitSignState |= (1 << U_CYL_POS_SIGN);
		}
		else
		{
			exitSignState &= ~(1 << U_CYL_POS_SIGN);
		}
		msg.dataType = U_CYL_POS_SIGN;
//		msg.pVoid = (void *)signState; //ֱ�Ӵ���ָ��ֵ;		
		msg.pVoid = &signState;
		
		if(me->cbHandle[EXT_SIGN_CHANGE]->callback)
		{
			me->cbHandle[EXT_SIGN_CHANGE]->callback(me->cbHandle[EXT_SIGN_CHANGE]->pInstance, &msg);
		}
		me->clearFlag(me, U_CYL_POS_SIGN);
	}
	//��⵽��������λ�����б仯
	if(me->changeFlag & 0x10)
	{
		signState = me->getExitSign(me, D_CYL_POS_SIGN);
		if(signState)
		{			
			exitSignState |= (1 << D_CYL_POS_SIGN);
		}
		else
		{
			exitSignState &= ~(1 << D_CYL_POS_SIGN);
		}
		msg.dataType = D_CYL_POS_SIGN;
//		msg.pVoid = (void *)signState; //ֱ�Ӵ���ָ��ֵ;		
		msg.pVoid = &signState;
		
		if(me->cbHandle[EXT_SIGN_CHANGE]->callback)
		{
			me->cbHandle[EXT_SIGN_CHANGE]->callback(me->cbHandle[EXT_SIGN_CHANGE]->pInstance, &msg);
		}
		me->clearFlag(me, D_CYL_POS_SIGN);
	}
	//��⵽����ǰλ����λ�����б仯
	if(me->changeFlag & 0x20)
	{
		signState = me->getExitSign(me, F_CYL_POS_SIGN);
		if(signState)
		{			
			exitSignState |= (1 << F_CYL_POS_SIGN);
		}
		else
		{
			exitSignState &= ~(1 << F_CYL_POS_SIGN);
		}
		msg.dataType = F_CYL_POS_SIGN;
//		msg.pVoid = (void *)signState; //ֱ�Ӵ���ָ��ֵ;		
		msg.pVoid = &signState;
		
		if(me->cbHandle[EXT_SIGN_CHANGE]->callback)
		{
			me->cbHandle[EXT_SIGN_CHANGE]->callback(me->cbHandle[EXT_SIGN_CHANGE]->pInstance, &msg);
		}
		me->clearFlag(me, F_CYL_POS_SIGN);
	}
	//��⵽���׺�λ����λ�����б仯
	if(me->changeFlag & 0x40)
	{
		signState = me->getExitSign(me, B_CYL_POS_SIGN);
		if(signState)
		{			
			exitSignState |= (1 << B_CYL_POS_SIGN);
		}
		else
		{
			exitSignState &= ~(1 << B_CYL_POS_SIGN);
		}
		msg.dataType = B_CYL_POS_SIGN;
//		msg.pVoid = (void *)signState; //ֱ�Ӵ���ָ��ֵ;		
		msg.pVoid = &signState;
		
		if(me->cbHandle[EXT_SIGN_CHANGE]->callback)
		{
			me->cbHandle[EXT_SIGN_CHANGE]->callback(me->cbHandle[EXT_SIGN_CHANGE]->pInstance, &msg);
		}
		me->clearFlag(me, B_CYL_POS_SIGN);
	}
	//��⵽��դ�б仯
	if(me->changeFlag & 0x80)
	{
		signState = me->getExitSign(me, GRAT_SIGN);
		if(signState)
		{			
			exitSignState |= (1 << GRAT_SIGN);
		}
		else
		{
			exitSignState &= ~(1 << GRAT_SIGN);
		}
		msg.dataType = GRAT_SIGN;
//		msg.pVoid = (void *)signState; //ֱ�Ӵ���ָ��ֵ;		
		msg.pVoid = &signState;
		
		if(me->cbHandle[EXT_SIGN_CHANGE]->callback)
		{
			me->cbHandle[EXT_SIGN_CHANGE]->callback(me->cbHandle[EXT_SIGN_CHANGE]->pInstance, &msg);
		}
		me->clearFlag(me, GRAT_SIGN);
	}
	//��modbus������Ϣ���´�����״̬
	if(me->cbHandle[EXT_SIGN_UPDATE]->callback)
	{
		me->cbHandle[EXT_SIGN_UPDATE]->callback(me->cbHandle[EXT_SIGN_UPDATE]->pInstance, &exitSignState);
	}
}

static void ExitSign_registerCallback(ExitSignal * me, void * pInshatnce, pGeneralCb p, ExtSigCb_t type)
{
	me->cbHandle[type]->callback = p;
	me->cbHandle[type]->pInstance = pInshatnce;
}


