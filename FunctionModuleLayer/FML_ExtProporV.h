#ifndef _FML_EXI_PROPOR_V_H
#define _FML_EXI_PROPOR_V_H
#include "universalType.h"


typedef struct _ProporValdDev ProporValDev;
struct _ProporValdDev
{
	void (* init)(ProporValDev * me);
	void (* setOutputPres)(ProporValDev * me, float pres);
	void (* setMaxPres)(float pres);
	float (* getMaxPres)(void);
	void (* registerCallback)(ProporValDev * me, void * pInshatnce, pGeneralCb p); //注册回调函数
	
	float inputVoltage;		//输入电压
	float outputPres;		//输出压力
	float openPercent;		//阀门开度百分比
	
	NotifyHandle PropDisp_cbHandle;
};

ProporValDev * ProporValDev_create(void);

#endif

