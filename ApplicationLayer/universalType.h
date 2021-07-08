#ifndef UNIVERSAL_TYPE_H
#define UNIVERSAL_TYPE_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "rtos_timers.h"
#include "event_groups.h"
#include "sys.h"


/*��ͬ�߳�֮�䴫��������ʹ�õĵĽṹ�壬��������������Ҫͨ��˫�����ж���*/
typedef struct _AppMessage
{
	u32 dataType;	//��������(ͨ��˫���������Ͷ�Ӧ��ʱ͸����)
	void *pVoid;	//����ָ��
}AppMessage;


/**ͨ�ûص�������ԭ�ͻص�����**/
typedef void (* pGeneralCb)(void *, void *);
typedef struct _NotifyHandle
{
	pGeneralCb callback;	//�ص�����
	void *pInstance;		//�ص�������ʹ�ö���
}NotifyHandle;

typedef struct 
{
	float tor13;	//ȡ13Torʱ��
	float productDetc;	//��Ʒ���ʱ��
	float pipeAirPurge;		//��·����ʱ��
	float pipeAirPump;	//��·����ʱ��
	float pipeIsDry;		//��·����ʱ��
	float lowAirPump;	//����ʱ��	
	float lowBalance;	//��ѹƽ��ʱ��
	float measure;	//����ʱ��
	float measDry;		//����ʱ��
	float addPres;	//��ѹʱ��
	float waterDrain;		//��ˮʱ��
	float waterPump;	//��ˮʱ��
	float waterInTimeOut;	//עˮ��ʱʱ��
	float addPresTimeOut;	//��ѹ��ʱʱ��
	float HighBalance;	//��ѹƽ��ʱ��
	float HighAirPump;	//��ѹ����ʱ��
	float reverse_1;	//����1
	float reverse_2;	//����2
	float reverse_3;	//����3
	float reverse_4;	//����4 
	float reverse_5;	//����5
	float reverse_6;	//����6
}ProcessTimePara_t;

typedef struct
{
	float lowLeakRate; 	//��ѹй©��
	float highLeakRate;	//��ѹй¶��
	float dryLimit;		//�����ж���
	float lowWaterMax;	//΢©ˮѹ����
	float lowWaterMin;	//΢©ˮѹ����
	float highWaterMax;	//��©ˮѹ����s
	float highWaterMin;	//��©ˮѹ����
	float compensitionV_1;	//#1����
	float compensitionV_2;	//#2����
	float compensitionV_3;	//#3����
	float compensitionV_4;	//#4����
	float lowWaterOpening;	//��ѹ���ӱ���������
	float highWaterOpening;	//��ѹ���ӱ���������
	float existLimit;	//���ڲ�Ʒ�ж���
	float lowerLimit_L;	//��ѹ����
	float lowerLimit_H;	//��ѹ����
	float reverse_1;	//����1
	float reverse_2;	//����2
	float reverse_3;	//����3
	float reverse_4;	//����4
		
}PresPara_t;

typedef struct
{
	ProcessTimePara_t paraTime;
	PresPara_t paraPres;
}ParaSetting_t;

#endif

