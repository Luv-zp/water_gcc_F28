#ifndef _BLL_TEST_H
#define _BLL_TEST_H
#include "FML_ExtValve.h"
#include "FML_DataAcq.h"
#include "FML_DelayTime.h"
#include "FML_ExtProporV.h"
#include "FML_ExtSignal.h"
#include "BLL_SubLeakTest.h"

/*设备状态*/
typedef enum
{
	STATE_STOP = 0,		//待机状态
	STATE_CYLINDER_FORWARD,		//气缸前进
	STATE_CYLINDER_DOWN,	//气缸下压
	STATE_13TOR,	//取13Tor
	STATE_PRODUCT_TEST,		//产品检测
	STATE_AIR_PURGE,	//管路吹气
	STATE_AIR_PUMP,		//管路抽气、
	STATE_IS_DRY, 		//干燥判断
	STATE_WATER_IN,		//注水
	STATE_ADD_PRES,		//加压
	STATE_LEAK_TEST,	//泄露检测
	STATE_WATER_OUT,	//排水
	STATE_WATER_PUMP,	//抽水
	STATE_CYLINDER_UP,  //气缸上抬
	STATE_CYLINDER_BACK, //气缸回退	
}TestStateType_t;

typedef struct _TestProcessor TestProcessor;
struct _TestProcessor
{
	void (* init)(TestProcessor * const me);
	TestStateType_t (* getCurrentState)(TestProcessor * const me);
	/*多事件接收器，单例，故不需要指向对象自身的指针*/
	void (* evInit)(void);			//初始化事件
	void (* evStart)(void);			//启动事件
	void (* evStop)(void);			//停止事件
	void (* evDelayOver)(void);		//延时结束
	void (* evTimeOut)(void);		//超时
	void (* evLiqSwitch)(u8 type);	//液位报警
	void (* evCylPosU)(u8 type);	//气缸上限位传感器
	void (* evCylPosD)(u8 type);	//气缸下限位传感器
	void (* evCylPosF)(u8 type);	//气缸前限位传感器
	void (* evCylPosB)(u8 type);	//气缸后限位传感器
	void (* evGrat)(u8 type);	//光栅
	
	ValveIODev * valveDev;			//阀门设备对象
	DataServer * adDev;				//数据采集设备对象
	ProporValDev * proporV;			//比例阀对象
	ExitSignal * exitSign;			//外部开关传感器对象	
	DelayTimer * delayTimer;		//延时定时器
	DelayTimer * outTimer;			//超时定时器
	
	LeakTestProcessor * its_pSubLeakTestProcessor;	//子泄漏测试状态机
	
	ProcessTimePara_t * timePara;		//时间参数
	PresPara_t * presPara;			//压力参数
	
	TestStateType_t stateID;
	float measPresRef;			//泄漏参考值
	u8 isInitFlag;
};
TestProcessor * TestProcessor_create(void);

#endif

