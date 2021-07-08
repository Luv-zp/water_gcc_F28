#ifndef _FML_DELAY_TIME_H
#define _FML_DELAY_TIME_H
#include "universalType.h"


typedef enum
{
	D_TIM_1,
	D_TIM_2,
	D_TIM_3,
	D_TIM_MAX
}DelayTimeType;

/*回调函数类型*/
typedef enum
{
	TIME_DISP,		//数据显示
	TIME_OVER,		//时间结束
	TIME_REACT,		//状态动作
	TIME_CB_MAX
}DelayTimeCbType;

typedef struct _DelayTimer DelayTimer;
struct _DelayTimer
{
	void (* init)(DelayTimer * me);
	void (* setDelayTime)(DelayTimer * me, float time);
	void (* stopTimer)(DelayTimer * me);
	void (* registerCallback)(DelayTimer * me, void * pInstance, const pGeneralCb p, DelayTimeCbType type);
	void (* unregisterCallback)(DelayTimer * me, DelayTimeCbType type);
	float delayTime;
	float nowTime;
	DelayTimeType timerType;
	
	TimerHandle_t timerHandle;
	NotifyHandle * cbHandle[TIME_CB_MAX];	//保存指向回调函数句柄的指针
	u8 isInitFlag;
};
DelayTimer * DelayTimer_create(DelayTimeType timerType);

#endif


