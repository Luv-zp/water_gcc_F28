#ifndef _FML_EXT_SIGNAL_H
#define _FML_EXT_SIGNAL_H
#include "key.h"
#include "universalType.h"

//回调函数类型
typedef enum
{
	EXT_SIGN_CHANGE,	//信号有变化
	EXT_SIGN_UPDATE,	//信号显示
	EXT_CB_MAX
}ExtSigCb_t;

//外部信号量类（类的组合）
typedef struct _ExitSignal ExitSignal;
struct _ExitSignal
{
	void (* init)(ExitSignal * me);	//初始化
	void (* signDetect)(ExitSignal * me);	//检测外部信号
	u8 (* getExitSign)(ExitSignal * me, ExitSign_t type); //获取指定信号量的状态
	void (* clearFlag)(ExitSignal * me, ExitSign_t type);	//清除标志位
	void (* registerCallback)(ExitSignal * me, void * pInshatnce, pGeneralCb p, ExtSigCb_t type); //注册回调函数
	
	/* 无私有数据成员 */
	SwitchSensor * switchSensorArr[SIGN_NUM_MAX];
	SwitchSensorState_t signStateArr[SIGN_NUM_MAX];
	u8 changeFlag;
	u8 maskBit;
	NotifyHandle * cbHandle[EXT_CB_MAX];
	
	u8 isInitFlag;
};

ExitSignal * ExitSignal_create(void);

#endif

