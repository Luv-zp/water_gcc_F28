#ifndef _LED_H
#define _LED_H
#include "sys.h"


//LED端口定义
#define LED0(n)		(n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET))
#define LED0_Toggle (HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1))
#define LED1(n)		(n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET))
#define LED1_Toggle (HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0))

void LED_Init(void);

/* 电磁阀IO端口引脚定义 */

#define V_1_PIN			GPIO_PIN_2
#define V_1_PORT	GPIOE
#define V_2_PIN			GPIO_PIN_4
#define V_2_PORT	GPIOE
#define V_3_PIN			GPIO_PIN_6
#define V_3_PORT	GPIOA
#define V_4_PIN			GPIO_PIN_3
#define V_4_PORT	GPIOE
#define V_5_PIN			GPIO_PIN_5
#define V_5_PORT	GPIOE
#define V_6_PIN			GPIO_PIN_13
#define V_6_PORT	GPIOD
#define V_7_PIN			GPIO_PIN_4
#define V_7_PORT	GPIOD
#define V_8_PIN			GPIO_PIN_3
#define V_8_PORT	GPIOD
#define V_9_PIN			GPIO_PIN_7
#define V_9_PORT	GPIOD
#define V_10_PIN		GPIO_PIN_12
#define V_10_PORT	GPIOD
#define V_11_PIN		GPIO_PIN_5
#define V_11_PORT	GPIOD
#define V_12_PIN		GPIO_PIN_4
#define V_12_PORT	GPIOA
#define V_13_PIN		GPIO_PIN_11
#define V_13_PORT	GPIOD
#define V_14_PIN		GPIO_PIN_7
#define V_14_PORT	GPIOC
#define V_15_PIN		GPIO_PIN_10
#define V_15_PORT	GPIOG
#define V_16_PIN		GPIO_PIN_5
#define V_16_PORT	GPIOA
#define V_17_PIN		GPIO_PIN_8
#define V_17_PORT	GPIOA
#define V_18_PIN		GPIO_PIN_6
#define V_18_PORT	GPIOE
#define V_19_PIN		GPIO_PIN_6
#define V_19_PORT	GPIOC
#define V_20_PIN		GPIO_PIN_13
#define V_20_PORT	GPIOH
#define V_21_PIN		GPIO_PIN_14
#define V_21_PORT	GPIOH
#define V_22_PIN		GPIO_PIN_15
#define V_22_PORT	GPIOH
#define V_23_PIN		GPIO_PIN_3
#define V_23_PORT	GPIOA
#define V_24_PIN		GPIO_PIN_12
#define V_24_PORT	GPIOH


/* 电磁阀种类 */
typedef enum
{
	WATER_IN_V = 0,		//注水阀
	PRES_TO_WATER_V,	//加压阀
	PUMP_WATER_V,		//抽水阀
	AIR_IN_V,			//进气阀
	WATER_OUT_V,		//排水阀
	DRY_V,				//干燥阀
	BIG_VACUUM_V,		//大真空阀
	BIG_LEAK_1_V,		//大泄漏阀1
	BIG_LEAK_2_V,		//大泄漏阀2
	BIG_LEAK_3_V,		//大泄漏阀3
	BIG_LEAK_4_V,		//大泄漏阀4
	LIT_VACUUM_V,		//微真空阀
	LIT_LEAK_1_V,		//微泄漏阀1
	LIT_LEAK_2_V,		//微泄漏阀2
	LIT_LEAK_3_V,		//微泄漏阀3
	LIT_LEAK_4_V,		//微泄漏阀4
	AIR_OUT_V,			//排气阀
	NEEDLE_V,			//针阀开关
	BALANCE_V,			//平衡阀
	WATER_PUMP,			//水泵
	CYL_UP_V,			//气缸上进气开关
	CYL_DOWN_V,			//气缸下进气开关
	CYL_BACK_V,			//气缸后进气开关
	CYL_FRONT_V,		//气缸前进气开关
	MAX_NUM_V,	//电磁阀数量
}valveIO_t;

//电磁阀类
typedef struct _MagneticValve MagneticValve;
struct _MagneticValve
{
	void (* init)(MagneticValve * me, GPIO_TypeDef * port, uint16_t pin, uint8_t onLevel);
	void (* setState)(MagneticValve * me, uint8_t state);
	u8 (* getState)(MagneticValve * me);
	void * private_data;		//私有成员
};

MagneticValve * MagneticValve_create(valveIO_t valve_type);	//相当于构造函数

#endif
