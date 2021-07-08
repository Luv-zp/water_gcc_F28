#ifndef _FML_DATA_ACQ_H
#define _FML_DATA_ACQ_H
#include "sys.h"


typedef enum
{
	CUR_WATER_DATA,		//实时水压
	PRO_DEC_WATER,		//产品检测阶段水压
	AFTER_ADD_WATER		//加压后得水压
}PrestDataType;

typedef struct _DataServer DataServer;
struct _DataServer
{
	void (* init)(DataServer * const me);		//指针常量，指针本身的值不能改变
	float (* getPresData)(DataServer * const me, PrestDataType dataType);	//读取压力数据
	
	float waterPres;		//水压
};

DataServer * DataServer_create(void);
#endif

