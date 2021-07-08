#include "FML_ExtProporV.h"
#include "timer.h"

static void ProPor_init(ProporValDev * me);
static void Propor_setOutputPres(ProporValDev * me, float pres); //设置输出压力
static void Propor_setMaxPres(float pres);	//设置气源压力
static float Propor_getMaxPres(void);
static void Propor_registerCallback(ProporValDev * me, void * pInshatnce, pGeneralCb p); //注册回调函数

float maxPresSource = 0; //气源最大压力
ProporValDev proporValDev;

ProporValDev * ProporValDev_create(void)
{
	static ProporValDev * me = 0;
	if(me == 0)
	{
		me = &proporValDev;
		me->init = ProPor_init;
		me->setOutputPres = Propor_setOutputPres;
		me->setMaxPres = Propor_setMaxPres;
		me->getMaxPres = Propor_getMaxPres;
		me->registerCallback = Propor_registerCallback;
		me->outputPres = 0;
		me->inputVoltage = 0;
		me->openPercent = 0;
		
		maxPresSource = 700;
		me->init(me);
	}
	return me;
}

static void ProPor_init(ProporValDev * me)
{
	/*计数频率108 / 108 = 1 	MHz, 自动重装载值 500， 所以PWM频率为 2KHz*/
	TIM3_PWM_Init(1000 - 1, 54 - 1);
}

//设置阀门开度
//opening: 阀门开度（0~1）
static void Propor_setOutputPres(ProporValDev * me, float opening)
{
	int out = 0;
	out = 1000 - opening * 1000.0f;
	if(out > 1000) out = 1000;
	if(out < 0) out = 0;
	TIM_SetTIM3Compare4(out);
	me->outputPres = opening;
	me->openPercent = opening * 100;
	
	//调用回调函数更新当前阀门开度
	if(me->PropDisp_cbHandle.callback)
	{
		me->PropDisp_cbHandle.callback(me->PropDisp_cbHandle.pInstance, &me->openPercent);
	}
}

static void Propor_setMaxPres(float pres) //设置气源压力
{
	maxPresSource = pres;
}


static float Propor_getMaxPres(void)
{
	return maxPresSource;
}

static void Propor_registerCallback(ProporValDev * me, void * pInshatnce, pGeneralCb p) //注册回调函数
{
	me->PropDisp_cbHandle.callback = p;
	me->PropDisp_cbHandle.pInstance = pInshatnce;
}

