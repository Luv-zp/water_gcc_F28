#include "key.h"
#include "delay.h"


typedef struct
{
	GPIO_TypeDef * port;
	u16 pin;
	ExitSign_t signType;
	VolTrig_t trigType;
	u32 trigCountLimit[2]; 	//触发或者解除触发计数判定限
	u32 trigCount[2];		//触发或者解除触发计数
	SwitchSensorState_t state;
}SwitchSensorData;

static void init(SwitchSensor * me);
static u8 signDetect(SwitchSensor * me, SwitchSensorState_t * state);	//信号检测
static void setTrigCountLimit(SwitchSensor * me, u32 trig_on, u32 trig_off);	//设置触发计数
static void setTrigType(SwitchSensor * me, VolTrig_t type);	//设置触发电平

static SwitchSensor switchSensor[SIGN_NUM_MAX];
static SwitchSensorData switchSensorData[SIGN_NUM_MAX];

GPIO_TypeDef * const portArr[SIGN_NUM_MAX] = {
	LIQ_SWITCH_PORT, VACUUM_SENS_PORT, PRES_SENS_PORT, CYL_UP_POS_PORT, CYL_DOWN_POS_PORT, CYL_FRONT_POS_PORT, CYL_BACK_POS_PORT, GRAT_PORT
};

const u16 pinArr[SIGN_NUM_MAX] = {
	LIQ_SWITCH_PIN, VACUUM_SENS_PIN, PRES_SENS_PIN, CYL_UP_POS_PIN, CYL_DOWN_POS_PIN, CYL_FRONT_POS_PIN, CYL_BACK_POS_PIN, GRAT_PIN
};


SwitchSensor * SwitchSensor_create(ExitSign_t type)
{
	SwitchSensor * me = &switchSensor[type];
	SwitchSensorData * pSwitchData = 0;
	if(me->init == 0)
	{
		me->init = init;
		me->signDetect = signDetect;
		me->setTrigType =setTrigType;
		me->setTrigCountLimit = setTrigCountLimit;
		
		me->private_data = &switchSensorData[type];
		pSwitchData = (SwitchSensorData *)me->private_data;
		pSwitchData->signType = type;	
		pSwitchData->port = portArr[type];
		pSwitchData->pin = pinArr[type];
		
		me->init(me);
	}
	return me;
}

static void init(SwitchSensor * me)
{
	SwitchSensorData * privateData = (SwitchSensorData *)me->private_data;
	GPIO_InitTypeDef GPIO_Initure;
	static u8 initFlag = 0;
	if(initFlag == 0) //保证只执行一次
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			//开启GPIOA时钟
		__HAL_RCC_GPIOB_CLK_ENABLE();			//开启GPIOB时钟
		__HAL_RCC_GPIOC_CLK_ENABLE();			//开启GPIOC时钟
		__HAL_RCC_GPIOG_CLK_ENABLE();			//开启GPIOE时钟
		__HAL_RCC_GPIOH_CLK_ENABLE();			//开启GPIOG时钟
		__HAL_RCC_GPIOI_CLK_ENABLE();			//开启GPIOH时钟
		initFlag = 1;
	}
	GPIO_Initure.Mode = GPIO_MODE_INPUT;	//推挽输入
	GPIO_Initure.Pull = GPIO_PULLDOWN;		//下拉 上拉电阻：保证IO没有外部信号输入时，STM32检测到是高电平，有信号时，跟随信号电平；
											//	   下拉电阻：保证IO没有信号输入时，STM32检测到是低电平，有信号时，跟随信号电平。
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;	//高速
	GPIO_Initure.Pin = privateData->pin;
	HAL_GPIO_Init(privateData->port, &GPIO_Initure);
	
	privateData->trigCount[TRIG_ON] = 0;
	privateData->trigCount[TRIG_OFF] = 0;
	privateData->state = TRIG_OFF;
}

//检测信号有无变化
//state 保存信号检测的结果
//返回：0 信号无变化，1 信号有变化
static u8 signDetect(SwitchSensor * me, SwitchSensorState_t * state)
{
	SwitchSensorData * privateData = (SwitchSensorData *)me->private_data;
	u8 changeFlag = 0;
	if(privateData->trigType == HAL_GPIO_ReadPin(privateData->port, privateData->pin))	//触发
	{
		privateData->trigCount[TRIG_ON] ++;
		privateData->trigCount[TRIG_OFF] = 0;
		if((privateData->state == TRIG_OFF) && (privateData->trigCount[TRIG_ON] >= privateData->trigCountLimit[TRIG_ON]))
		{
			privateData->state = TRIG_ON;
			changeFlag = 1;
		}
	}
	else
	{
		privateData->trigCount[TRIG_ON] = 0;
		privateData->trigCount[TRIG_OFF] ++;
		if((privateData->state == TRIG_ON) && (privateData->trigCount[TRIG_OFF] >= privateData->trigCountLimit[TRIG_OFF]))
		{
			privateData->state = TRIG_OFF;
			changeFlag = 1;
		}	
	}
	*state = privateData->state ;
	return changeFlag;
}


static void setTrigCountLimit(SwitchSensor * me, u32 trig_on, u32 trig_off)
{
	SwitchSensorData * privateData = (SwitchSensorData *)me->private_data;
	privateData->trigCountLimit[TRIG_ON] = trig_on;
	privateData->trigCountLimit[TRIG_OFF] = trig_off;
}


static void setTrigType(SwitchSensor * me, VolTrig_t type)
{
	SwitchSensorData * privateData = (SwitchSensorData *)me->private_data;
	privateData->trigType = type;
}





