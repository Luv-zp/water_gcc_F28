#include "FML_ExtValve.h"

typedef struct
{
	MagneticValve * pMagValveArr[MAX_NUM_V];
	u8 valveStateArr[MAX_NUM_V];
}MagValveManageData;

static NotifyHandle valve_cbHandle[CB_V_MAX];

static void valve_init(ValveIODev * const me);	//初始化电磁阀IO外设
static void valve_setValState(ValveIODev * const me, valveIO_t valve, u8 state);	//设置阀门状态
static u8 valve_getValState(ValveIODev * const me, valveIO_t valve);	//获取阀门状态
static void ExitSing_registerCallback(ValveIODev * me, void * pInstance, const pGeneralCb p, ValveCbType type);	//注册回调函数

static ValveIODev valveIODev;	//静态创建电磁阀管理器对象
static MagValveManageData s_MagValveManageData;

//阀门设备类（类的组合）
ValveIODev * ValveIODev_create(void)
{
	static ValveIODev * me = 0;
	int i = 0;
	if(me == 0)
	{
		me = &valveIODev;
		me->init = valve_init;
		me->setValState = valve_setValState;
		me->getValState = valve_getValState;
		me->registerCallback = ExitSing_registerCallback;
		
		for(i = 0; i < CB_V_MAX; i++)
		{
			me->cbHandle[i] = &valve_cbHandle[i];
		}
		me->private_data = &s_MagValveManageData;
		me->init(me);
	}
	return me;
}

static void valve_init(ValveIODev * const me)
{
	int i = 0;
	MagValveManageData * privateData = (MagValveManageData *)me->private_data;
	for(i = 0; i < MAX_NUM_V; i++)
	{
		privateData->pMagValveArr[i] = MagneticValve_create(i);
	}
	privateData->pMagValveArr[WATER_IN_V]->init(privateData->pMagValveArr[WATER_IN_V], V_1_PORT, V_1_PIN, 1);
	privateData->pMagValveArr[PRES_TO_WATER_V]->init(privateData->pMagValveArr[PRES_TO_WATER_V], V_2_PORT, V_2_PIN, 1);
	privateData->pMagValveArr[PUMP_WATER_V]->init(privateData->pMagValveArr[PUMP_WATER_V], V_3_PORT, V_3_PIN, 1);
	privateData->pMagValveArr[AIR_IN_V]->init(privateData->pMagValveArr[AIR_IN_V], V_4_PORT, V_4_PIN, 1);
	privateData->pMagValveArr[WATER_OUT_V]->init(privateData->pMagValveArr[WATER_OUT_V], V_5_PORT, V_5_PIN, 1);
	privateData->pMagValveArr[DRY_V]->init(privateData->pMagValveArr[DRY_V], V_6_PORT, V_6_PIN, 1);
	privateData->pMagValveArr[BIG_VACUUM_V]->init(privateData->pMagValveArr[BIG_VACUUM_V], V_7_PORT, V_7_PIN, 1);
	privateData->pMagValveArr[BIG_LEAK_1_V]->init(privateData->pMagValveArr[BIG_LEAK_1_V], V_8_PORT, V_8_PIN, 1);
	privateData->pMagValveArr[BIG_LEAK_2_V]->init(privateData->pMagValveArr[BIG_LEAK_2_V], V_9_PORT, V_9_PIN, 1);
	privateData->pMagValveArr[BIG_LEAK_3_V]->init(privateData->pMagValveArr[BIG_LEAK_3_V], V_10_PORT, V_10_PIN, 1);
	privateData->pMagValveArr[BIG_LEAK_4_V]->init(privateData->pMagValveArr[BIG_LEAK_4_V], V_11_PORT, V_11_PIN, 1);
	privateData->pMagValveArr[LIT_VACUUM_V]->init(privateData->pMagValveArr[LIT_VACUUM_V], V_12_PORT, V_12_PIN, 1);
	privateData->pMagValveArr[LIT_LEAK_1_V]->init(privateData->pMagValveArr[LIT_LEAK_1_V], V_13_PORT, V_13_PIN, 1);
	privateData->pMagValveArr[LIT_LEAK_2_V]->init(privateData->pMagValveArr[LIT_LEAK_2_V], V_14_PORT, V_14_PIN, 1);
	privateData->pMagValveArr[LIT_LEAK_3_V]->init(privateData->pMagValveArr[LIT_LEAK_3_V], V_15_PORT, V_15_PIN, 1);
	privateData->pMagValveArr[LIT_LEAK_4_V]->init(privateData->pMagValveArr[LIT_LEAK_4_V], V_16_PORT, V_16_PIN, 1);
	privateData->pMagValveArr[AIR_OUT_V]->init(privateData->pMagValveArr[AIR_OUT_V], V_17_PORT, V_17_PIN, 1);
	privateData->pMagValveArr[NEEDLE_V]->init(privateData->pMagValveArr[NEEDLE_V], V_18_PORT, V_18_PIN, 1);
	privateData->pMagValveArr[BALANCE_V]->init(privateData->pMagValveArr[BALANCE_V], V_19_PORT, V_19_PIN, 1);
	privateData->pMagValveArr[WATER_PUMP]->init(privateData->pMagValveArr[WATER_PUMP], V_20_PORT, V_20_PIN, 1);
	privateData->pMagValveArr[CYL_UP_V]->init(privateData->pMagValveArr[CYL_UP_V], V_21_PORT, V_21_PIN, 1);
	privateData->pMagValveArr[CYL_DOWN_V]->init(privateData->pMagValveArr[CYL_DOWN_V], V_22_PORT, V_22_PIN, 1);
	privateData->pMagValveArr[CYL_BACK_V]->init(privateData->pMagValveArr[CYL_BACK_V], V_23_PORT, V_23_PIN, 1);
	privateData->pMagValveArr[CYL_FRONT_V]->init(privateData->pMagValveArr[CYL_FRONT_V], V_24_PORT, V_24_PIN, 1);
}

static void valve_setValState(ValveIODev * const me, valveIO_t valve, u8 state)
{
	MagValveManageData * privateData = (MagValveManageData *)me->private_data;
	privateData->pMagValveArr[valve]->setState(privateData->pMagValveArr[valve], state);
	privateData->valveStateArr[valve] = state;
	
	/*阀门状态有变化，调用相应的回调函数*/
	if((me->cbHandle[CB_V_DISP]->pInstance != 0)&&(me->cbHandle[CB_V_DISP]->callback != 0))
	{
		me->cbHandle[CB_V_DISP]->callback(me->cbHandle[CB_V_DISP]->pInstance, privateData->valveStateArr);
	}
}

static u8 valve_getValState(ValveIODev * const me, valveIO_t valve)
{
	MagValveManageData * privateData = (MagValveManageData *)me->private_data;
	return privateData->valveStateArr[valve];
}

static void ExitSing_registerCallback(ValveIODev * me, void * pInstance, const pGeneralCb p, ValveCbType type)
{
	me->cbHandle[type]->pInstance = pInstance;
	me->cbHandle[type]->callback = p;
}

