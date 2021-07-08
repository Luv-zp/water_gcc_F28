#ifndef _BLL_TEST_H
#define _BLL_TEST_H
#include "FML_ExtValve.h"
#include "FML_DataAcq.h"
#include "FML_DelayTime.h"
#include "FML_ExtProporV.h"
#include "FML_ExtSignal.h"
#include "BLL_SubLeakTest.h"

/*�豸״̬*/
typedef enum
{
	STATE_STOP = 0,		//����״̬
	STATE_CYLINDER_FORWARD,		//����ǰ��
	STATE_CYLINDER_DOWN,	//������ѹ
	STATE_13TOR,	//ȡ13Tor
	STATE_PRODUCT_TEST,		//��Ʒ���
	STATE_AIR_PURGE,	//��·����
	STATE_AIR_PUMP,		//��·������
	STATE_IS_DRY, 		//�����ж�
	STATE_WATER_IN,		//עˮ
	STATE_ADD_PRES,		//��ѹ
	STATE_LEAK_TEST,	//й¶���
	STATE_WATER_OUT,	//��ˮ
	STATE_WATER_PUMP,	//��ˮ
	STATE_CYLINDER_UP,  //������̧
	STATE_CYLINDER_BACK, //���׻���	
}TestStateType_t;

typedef struct _TestProcessor TestProcessor;
struct _TestProcessor
{
	void (* init)(TestProcessor * const me);
	TestStateType_t (* getCurrentState)(TestProcessor * const me);
	/*���¼����������������ʲ���Ҫָ����������ָ��*/
	void (* evInit)(void);			//��ʼ���¼�
	void (* evStart)(void);			//�����¼�
	void (* evStop)(void);			//ֹͣ�¼�
	void (* evDelayOver)(void);		//��ʱ����
	void (* evTimeOut)(void);		//��ʱ
	void (* evLiqSwitch)(u8 type);	//Һλ����
	void (* evCylPosU)(u8 type);	//��������λ������
	void (* evCylPosD)(u8 type);	//��������λ������
	void (* evCylPosF)(u8 type);	//����ǰ��λ������
	void (* evCylPosB)(u8 type);	//���׺���λ������
	void (* evGrat)(u8 type);	//��դ
	
	ValveIODev * valveDev;			//�����豸����
	DataServer * adDev;				//���ݲɼ��豸����
	ProporValDev * proporV;			//����������
	ExitSignal * exitSign;			//�ⲿ���ش���������	
	DelayTimer * delayTimer;		//��ʱ��ʱ��
	DelayTimer * outTimer;			//��ʱ��ʱ��
	
	LeakTestProcessor * its_pSubLeakTestProcessor;	//��й©����״̬��
	
	ProcessTimePara_t * timePara;		//ʱ�����
	PresPara_t * presPara;			//ѹ������
	
	TestStateType_t stateID;
	float measPresRef;			//й©�ο�ֵ
	u8 isInitFlag;
};
TestProcessor * TestProcessor_create(void);

#endif

