#ifndef _KEY_H
#define _KEY_H
#include "sys.h"

/*k开关型传感器引脚定义*/
#define LIQ_SWITCH_PIN		GPIO_PIN_11
#define LIQ_SWITCH_PORT		GPIOI
#define VACUUM_SENS_PIN		GPIO_PIN_2
#define VACUUM_SENS_PORT	GPIOH
#define PRES_SENS_PIN		GPIO_PIN_8
#define PRES_SENS_PORT		GPIOH
#define CYL_UP_POS_PIN		GPIO_PIN_0
#define CYL_UP_POS_PORT		GPIOB
#define CYL_DOWN_POS_PIN	GPIO_PIN_3
#define CYL_DOWN_POS_PORT	GPIOH
#define CYL_FRONT_POS_PIN	GPIO_PIN_13
#define CYL_FRONT_POS_PORT	GPIOC
#define CYL_BACK_POS_PIN	GPIO_PIN_12
#define CYL_BACK_POS_PORT	GPIOA
#define GRAT_PIN			GPIO_PIN_11
#define GRAT_PORT			GPIOA

/*外部开关型信号量定义*/
typedef enum
{
	WATER_DETEC_SIGN = 0,	//液位开关
	VACUUM_SIGN,			//真空表
	AIR_PRES_SIGN,			//压力表
	U_CYL_POS_SIGN,			//气缸上到位
	D_CYL_POS_SIGN,			//气缸下到位
	F_CYL_POS_SIGN,			//气缸前到位
	B_CYL_POS_SIGN,			//气缸后到位
	GRAT_SIGN,				//光栅
	SIGN_NUM_MAX			//信号量总数
}ExitSign_t;

//传感器触发方式
typedef enum
{
	LOW_LEVEL_TRIG = 0,		//低电平触发
	HIGH_LEVEL_TRIG			//高电平触发
}VolTrig_t;

//传感器状态
typedef enum
{
	TRIG_OFF, 	//传感器未触发
	TRIG_ON,	//触发
}SwitchSensorState_t;

//开关传感器类
typedef struct _SwitchSensor SwitchSensor;
struct _SwitchSensor
{
	void (* init)(SwitchSensor * me);
	u8 (* signDetect)(SwitchSensor * me, SwitchSensorState_t * state); //信号检测
	void (* setTrigType)(SwitchSensor * me, VolTrig_t type); //设置触发电平
	void (* setTrigCountLimit)(SwitchSensor * me, u32 trig_on, u32 trig_off);
	
	void * private_data; //私有数据
};

SwitchSensor * SwitchSensor_create(ExitSign_t type);

#endif
