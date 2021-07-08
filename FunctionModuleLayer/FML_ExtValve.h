#ifndef _FML_EXT_VALVE_H
#define _FML_EXT_VALVE_H
#include "led.h"
#include "universalType.h"

typedef enum
{
	CB_V_DISP,	//显示阀门状态
	CB_V_MAX
}ValveCbType;

//阀门设备类（类的组合）
typedef struct _ValveIODev ValveIODev;
struct _ValveIODev
{
	void (* init)(ValveIODev * const me);	//初始化电磁阀IO外设
	void (* setValState)(ValveIODev * const me, valveIO_t valve, u8 state);	//设置阀门状态
	u8 (* getValState)(ValveIODev * const me, valveIO_t valve);	//获取阀门状态
	void (* registerCallback)(ValveIODev * me, void * pInstance, const pGeneralCb p, ValveCbType type);	//注册回调函数
	
	NotifyHandle * cbHandle[CB_V_MAX];	//指向所有通知句柄的指针数组
	void * private_data;
};

ValveIODev * ValveIODev_create(void);



#endif

