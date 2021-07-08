#ifndef _FML_EXT_VALVE_H
#define _FML_EXT_VALVE_H
#include "led.h"
#include "universalType.h"

typedef enum
{
	CB_V_DISP,	//��ʾ����״̬
	CB_V_MAX
}ValveCbType;

//�����豸�ࣨ�����ϣ�
typedef struct _ValveIODev ValveIODev;
struct _ValveIODev
{
	void (* init)(ValveIODev * const me);	//��ʼ����ŷ�IO����
	void (* setValState)(ValveIODev * const me, valveIO_t valve, u8 state);	//���÷���״̬
	u8 (* getValState)(ValveIODev * const me, valveIO_t valve);	//��ȡ����״̬
	void (* registerCallback)(ValveIODev * me, void * pInstance, const pGeneralCb p, ValveCbType type);	//ע��ص�����
	
	NotifyHandle * cbHandle[CB_V_MAX];	//ָ������֪ͨ�����ָ������
	void * private_data;
};

ValveIODev * ValveIODev_create(void);



#endif

