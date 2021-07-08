#ifndef _FML_DATA_ACQ_H
#define _FML_DATA_ACQ_H
#include "sys.h"


typedef enum
{
	CUR_WATER_DATA,		//ʵʱˮѹ
	PRO_DEC_WATER,		//��Ʒ���׶�ˮѹ
	AFTER_ADD_WATER		//��ѹ���ˮѹ
}PrestDataType;

typedef struct _DataServer DataServer;
struct _DataServer
{
	void (* init)(DataServer * const me);		//ָ�볣����ָ�뱾���ֵ���ܸı�
	float (* getPresData)(DataServer * const me, PrestDataType dataType);	//��ȡѹ������
	
	float waterPres;		//ˮѹ
};

DataServer * DataServer_create(void);
#endif

