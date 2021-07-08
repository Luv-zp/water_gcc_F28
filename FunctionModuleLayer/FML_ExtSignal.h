#ifndef _FML_EXT_SIGNAL_H
#define _FML_EXT_SIGNAL_H
#include "key.h"
#include "universalType.h"

//�ص���������
typedef enum
{
	EXT_SIGN_CHANGE,	//�ź��б仯
	EXT_SIGN_UPDATE,	//�ź���ʾ
	EXT_CB_MAX
}ExtSigCb_t;

//�ⲿ�ź����ࣨ�����ϣ�
typedef struct _ExitSignal ExitSignal;
struct _ExitSignal
{
	void (* init)(ExitSignal * me);	//��ʼ��
	void (* signDetect)(ExitSignal * me);	//����ⲿ�ź�
	u8 (* getExitSign)(ExitSignal * me, ExitSign_t type); //��ȡָ���ź�����״̬
	void (* clearFlag)(ExitSignal * me, ExitSign_t type);	//�����־λ
	void (* registerCallback)(ExitSignal * me, void * pInshatnce, pGeneralCb p, ExtSigCb_t type); //ע��ص�����
	
	/* ��˽�����ݳ�Ա */
	SwitchSensor * switchSensorArr[SIGN_NUM_MAX];
	SwitchSensorState_t signStateArr[SIGN_NUM_MAX];
	u8 changeFlag;
	u8 maskBit;
	NotifyHandle * cbHandle[EXT_CB_MAX];
	
	u8 isInitFlag;
};

ExitSignal * ExitSignal_create(void);

#endif

