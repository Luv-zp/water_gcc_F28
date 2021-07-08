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
	void (* registerCallback)(ProporValDev * me, void * pInshatnce, pGeneralCb p); //ע��ص�����
	
	float inputVoltage;		//�����ѹ
	float outputPres;		//���ѹ��
	float openPercent;		//���ſ��Ȱٷֱ�
	
	NotifyHandle PropDisp_cbHandle;
};

ProporValDev * ProporValDev_create(void);

#endif

