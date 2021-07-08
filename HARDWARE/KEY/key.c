#include "key.h"
#include "delay.h"


typedef struct
{
	GPIO_TypeDef * port;
	u16 pin;
	ExitSign_t signType;
	VolTrig_t trigType;
	u32 trigCountLimit[2]; 	//�������߽�����������ж���
	u32 trigCount[2];		//�������߽����������
	SwitchSensorState_t state;
}SwitchSensorData;

static void init(SwitchSensor * me);
static u8 signDetect(SwitchSensor * me, SwitchSensorState_t * state);	//�źż��
static void setTrigCountLimit(SwitchSensor * me, u32 trig_on, u32 trig_off);	//���ô�������
static void setTrigType(SwitchSensor * me, VolTrig_t type);	//���ô�����ƽ

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
	if(initFlag == 0) //��ִֻ֤��һ��
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			//����GPIOAʱ��
		__HAL_RCC_GPIOB_CLK_ENABLE();			//����GPIOBʱ��
		__HAL_RCC_GPIOC_CLK_ENABLE();			//����GPIOCʱ��
		__HAL_RCC_GPIOG_CLK_ENABLE();			//����GPIOEʱ��
		__HAL_RCC_GPIOH_CLK_ENABLE();			//����GPIOGʱ��
		__HAL_RCC_GPIOI_CLK_ENABLE();			//����GPIOHʱ��
		initFlag = 1;
	}
	GPIO_Initure.Mode = GPIO_MODE_INPUT;	//��������
	GPIO_Initure.Pull = GPIO_PULLDOWN;		//���� �������裺��֤IOû���ⲿ�ź�����ʱ��STM32��⵽�Ǹߵ�ƽ�����ź�ʱ�������źŵ�ƽ��
											//	   �������裺��֤IOû���ź�����ʱ��STM32��⵽�ǵ͵�ƽ�����ź�ʱ�������źŵ�ƽ��
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;	//����
	GPIO_Initure.Pin = privateData->pin;
	HAL_GPIO_Init(privateData->port, &GPIO_Initure);
	
	privateData->trigCount[TRIG_ON] = 0;
	privateData->trigCount[TRIG_OFF] = 0;
	privateData->state = TRIG_OFF;
}

//����ź����ޱ仯
//state �����źż��Ľ��
//���أ�0 �ź��ޱ仯��1 �ź��б仯
static u8 signDetect(SwitchSensor * me, SwitchSensorState_t * state)
{
	SwitchSensorData * privateData = (SwitchSensorData *)me->private_data;
	u8 changeFlag = 0;
	if(privateData->trigType == HAL_GPIO_ReadPin(privateData->port, privateData->pin))	//����
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





