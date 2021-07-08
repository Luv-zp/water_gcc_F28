#ifndef UNIVERSAL_TYPE_H
#define UNIVERSAL_TYPE_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "rtos_timers.h"
#include "event_groups.h"
#include "sys.h"


/*不同线程之间传递数据所使用的的结构体，其中数据类型需要通信双方自行定义*/
typedef struct _AppMessage
{
	u32 dataType;	//数据类型(通信双方数据类型都应该时透明的)
	void *pVoid;	//数据指针
}AppMessage;


/**通用回调函数的原型回调函数**/
typedef void (* pGeneralCb)(void *, void *);
typedef struct _NotifyHandle
{
	pGeneralCb callback;	//回调函数
	void *pInstance;		//回调函数的使用对象
}NotifyHandle;

typedef struct 
{
	float tor13;	//取13Tor时间
	float productDetc;	//产品检测时间
	float pipeAirPurge;		//管路吹气时间
	float pipeAirPump;	//管路抽气时间
	float pipeIsDry;		//管路干燥时间
	float lowAirPump;	//抽气时间	
	float lowBalance;	//低压平衡时间
	float measure;	//测试时间
	float measDry;		//干燥时间
	float addPres;	//加压时间
	float waterDrain;		//排水时间
	float waterPump;	//抽水时间
	float waterInTimeOut;	//注水超时时间
	float addPresTimeOut;	//加压超时时间
	float HighBalance;	//高压平衡时间
	float HighAirPump;	//高压抽气时间
	float reverse_1;	//保留1
	float reverse_2;	//保留2
	float reverse_3;	//保留3
	float reverse_4;	//保留4 
	float reverse_5;	//保留5
	float reverse_6;	//保留6
}ProcessTimePara_t;

typedef struct
{
	float lowLeakRate; 	//低压泄漏量
	float highLeakRate;	//高压泄露量
	float dryLimit;		//干燥判定限
	float lowWaterMax;	//微漏水压上限
	float lowWaterMin;	//微漏水压下限
	float highWaterMax;	//大漏水压下限s
	float highWaterMin;	//大漏水压上限
	float compensitionV_1;	//#1补偿
	float compensitionV_2;	//#2补偿
	float compensitionV_3;	//#3补偿
	float compensitionV_4;	//#4补偿
	float lowWaterOpening;	//低压电子比例阀开度
	float highWaterOpening;	//高压电子比例阀开度
	float existLimit;	//存在产品判定限
	float lowerLimit_L;	//低压下限
	float lowerLimit_H;	//高压下限
	float reverse_1;	//保留1
	float reverse_2;	//保留2
	float reverse_3;	//保留3
	float reverse_4;	//保留4
		
}PresPara_t;

typedef struct
{
	ProcessTimePara_t paraTime;
	PresPara_t paraPres;
}ParaSetting_t;

#endif

