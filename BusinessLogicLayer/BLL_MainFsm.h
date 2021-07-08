#ifndef _BLL_MAIN_FSM_H
#define _BLL_MAIN_FSM_H
#include "userApp.h"
#include "BLL_Test.h"
#include "BLL_Debug.h"
#include "BLL_Dry.h"
 
#define TIME_DISP_TIM	D_TIM_1		//������ʾ����ʱ�Ķ�ʱ��
#define TIME_OUT_TIM	D_TIM_2		//���ڽ��г�ʱ��ʱ���Ķ�ʱ��

/* ����״̬�µ��豸״̬ */
typedef enum
{
	STOP_TEST = 0, 		//����
	STOP_DRY,			//����
	STOP_WATER_OUT,		//��ˮ
	STOP_DEBUG,			//����
}MainState_t;
 
/*����ɸı�״̬��״̬���¼�����*/
typedef enum
{
	EV_START = 0,	//��ʼ�ź�
	EV_STOP,		//ֹͣ�ź�
	EV_DELAY_OVER,	//��һ��
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

