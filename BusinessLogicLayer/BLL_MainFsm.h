#ifndef _BLL_MAIN_FSM_H
#define _BLL_MAIN_FSM_H
#include "userApp.h"
#include "BLL_Test.h"
#include "BLL_Debug.h"
#include "BLL_Dry.h"
 
#define TIME_DISP_TIM	D_TIM_1		//用于显示倒计时的定时器
#define TIME_OUT_TIM	D_TIM_2		//用于进行超时计时器的定时器

/* 待机状态下的设备状态 */
typedef enum
{
	STOP_TEST = 0, 		//测试
	STOP_DRY,			//干燥
	STOP_WATER_OUT,		//排水
	STOP_DEBUG,			//调试
}MainState_t;
 
/*定义可改变状态机状态的事件类型*/
typedef enum
{
	EV_START = 0,	//开始信号
	EV_STOP,		//停止信号
	EV_DELAY_OVER,	//下一步
	EV_LIQ_SWITCH,
	EV_VACUUM,
	EV_CYL_POS_U,
	EV_CYL_POS_D,
	EV_GRAT,
	EV_DRY_MODE,
	EV_DEBUG_MODE,
	EV_TIME_OUT,
	EV_SET_PARA,
	EV_SET_PROPOR,
	EV_SET_SIG_V
}EventType_t;

typedef struct _FiniteStateMachine FiniteStateMachine;
struct _FiniteStateMachine
{
	void (* init)(FiniteStateMachine * const me);
	void (* sendEvent)(AppMessage * msg);
	
	MainState_t mainStateID;
	TestProcessor * its_testProcessor;
	DebugProcessor * its_debugProcessor;
	DryProcessor * its_dryProcessor;
	u8 isInitFlag;
};

FiniteStateMachine * FiniteStateMachine_create(void);

#endif

