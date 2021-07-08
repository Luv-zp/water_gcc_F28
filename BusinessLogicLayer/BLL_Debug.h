#ifndef _BLL_DEBUG_H
#define _BLL_DEBUG_H
#include "FML_DelayTime.h"
#include "FML_ExtProporV.h"
#include "FML_ExtSignal.h"
#include "FML_ExtValve.h"

/* DEBUG状态下的子状态 */
typedef enum
{
	DEBUG_OFF = 0,		//待机
	DEBUG_ON,			//调试中
	DEBUG_CYL_UP		//气缸上抬
}DebugState_t;

typedef struct _DebugProcessor DebugProcessor;
struct _DebugProcessor
{
	void (* init)(DebugProcessor * const me);
	//多事件接收器，单例，故不需要指向对象自身的指针
	void (* evInit)(void);	 	//初始化事件
	void (* evstart)(void); 	//启动事件
	void (* evStop)(void);		//停止事件
	void (* evCylPosU)(u8 type);	//气缸上限位传感器响应
	void (* evGrat)(u8 type);		//光栅响应
	void (* evSetProporV)(void * pData);	//设置电子比例阀开度
	void (* evSetSingleV)(void * pData);	//设置单个阀门状态
	void (* evTimeOut)(void);	//超时
	
	DebugState_t  stateID;
	ValveIODev * pValve;
	ProporValDev * pProporV;
	ExitSignal * pExitSign;
	DelayTimer * pOutTimer;
	
	u8 isInitFlag;
};
DebugProcessor * DebugProcessor_create(void);


#endif

