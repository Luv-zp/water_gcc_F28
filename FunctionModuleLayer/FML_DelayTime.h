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

/*�ص���������*/
typedef enum
{
	TIME_DISP,		//������ʾ
	TIME_OVER,		//ʱ�����
	TIME_REACT,		//״̬����
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
	NotifyHandle * cbHandle[TIME_CB_MAX];	//����ָ��ص����������ָ��
	u8 isInitFlag;
};
DelayTimer * DelayTimer_create(DelayTimeType timerType);

#endif


