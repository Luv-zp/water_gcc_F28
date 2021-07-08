#ifndef _BLL_DRY_H
#define _BLL_DRY_H

#include "FML_DataAcq.h"
#include "FML_DelayTime.h"
#include "FML_ExtSignal.h"
#include "FML_ExtValve.h"
#include "BLL_SubLeakTest.h"



/* 干燥状态下的子状态 */
typedef enum
{
	DRY_NULL = 0,	//待机
	DRY_CYL_DOWN,
	DRY_CYL_UP,
	DRY_AIR_PURGE,
	DRY_AIR_PUMP,
	DRY_LEAK,			//漏气检测
}DryState_t;

typedef struct _DryProcessor DryProcessor;
struct _DryProcessor
{
	void (* init)(DryProcessor * const );
	void (* evInit)(void);	//进入默认伪状态
	void (* evStart)(void);
	void (* evStop)(void);
	void (* evDelayOver)(void);
	void (* evVacuum)(u8 type);
	void (* evCylPosU)(u8 type);
	void (* evCylPosD)(u8 type);
	void (* evGrat)(u8 type);
	void (* evTimeOut)(void);
	
	DryState_t stateID;
	ParaSetting_t * pParaSetting;
	
	ValveIODev * pValve;
	DataServer * pAD;
	ExitSignal * pExitSign;
	
	LeakTestProcessor * pLeakTest;
	DelayTimer * pDelayTimer;
	DelayTimer * pOutTimer;
	
	SemaphoreHandle_t mutex;
	u8 isInitFlag;
};
DryProcessor * DryProcessor_create(void);

#endif

