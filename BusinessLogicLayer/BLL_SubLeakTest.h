#ifndef _BLL_SUB_LEAK_TEST_H
#define _BLL_SUB_LEAK_TEST_H
#include "F28_FunctionCode.h"
#include "userApp.h"

typedef enum
{
	SUB_STATE_READY = 0,	//准备
	SUB_STATE_FILL,			//抽真空
	SUB_STATE_STAB,			//平衡
	SUB_STATE_TEST,			//测试
	SUB_STATE_DUMP,			//排气
	SUB_STATE_MAX			//状态计数
}LeakTestType_t;

typedef struct _LeakTestProcessor LeakTestProcessor;
struct _LeakTestProcessor
{
	void (* init)(LeakTestProcessor * me);
	/*多事件接收器，单例，故不需要指向对象自身的指针*/
	void (* evInit)(LeakTestProcessor * const me);
	void (* evStart)(LeakTestProcessor * const me);
	void (* evStop)(LeakTestProcessor * const me);
	
	LeakTestType_t (* getCurrentState)(LeakTestProcessor * me);
	float (* getPressure)(LeakTestProcessor * me);
	float (* getLeak)(LeakTestProcessor * me);
	void (* getResult)(LeakTestProcessor * const me);
	void (* SendFrame_GetLast)(LeakTestProcessor * const me);
	
	CF28Light * CF28Lpv;
	TimerHandle_t timerHandle;
	MBTCP_PROTOCOL_FRAME  MbTcp_ProtFrame;

	LeakTestType_t subState_ID; 
	float realtime_pressureValue;
	float realtime_leakValue;
	float last_pressureValue;
	float last_leakValue;
	u8 isInitFlag;
};
LeakTestProcessor * LeakTestProcessor_create(void);



#endif
