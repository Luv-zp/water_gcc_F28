#ifndef _BLL_DEBUG_H
#define _BLL_DEBUG_H
#include "FML_DelayTime.h"
#include "FML_ExtProporV.h"
#include "FML_ExtSignal.h"
#include "FML_ExtValve.h"

/* DEBUG״̬�µ���״̬ */
typedef enum
{
	DEBUG_OFF = 0,		//����
	DEBUG_ON,			//������
	DEBUG_CYL_UP		//������̧
}DebugState_t;

typedef struct _DebugProcessor DebugProcessor;
struct _DebugProcessor
{
	void (* init)(DebugProcessor * const me);
	//���¼����������������ʲ���Ҫָ����������ָ��
	void (* evInit)(void);	 	//��ʼ���¼�
	void (* evstart)(void); 	//�����¼�
	void (* evStop)(void);		//ֹͣ�¼�
	void (* evCylPosU)(u8 type);	//��������λ��������Ӧ
	void (* evGrat)(u8 type);		//��դ��Ӧ
	void (* evSetProporV)(void * pData);	//���õ��ӱ���������
	void (* evSetSingleV)(void * pData);	//���õ�������״̬
	void (* evTimeOut)(void);	//��ʱ
	
	DebugState_t  stateID;
	ValveIODev * pValve;
	ProporValDev * pProporV;
	ExitSignal * pExitSign;
	DelayTimer * pOutTimer;
	
	u8 isInitFlag;
};
DebugProcessor * DebugProcessor_create(void);


#endif

