#include "led.h"


//初始化PB0,PB1为输出.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOB_CLK_ENABLE();			//开启GPIOB时钟
	
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1; //PB0,1
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);	//PB0置0
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);	//PB1置1 
}

typedef struct
{
	u8 curState;			//current state
	u8 preState;			//previous state
	GPIO_TypeDef * port;	//gpio port
	u16 pin;				//gpio pin
	u8 onLevel; 			//voltage level when switch on the valve （高电平打开还是低电平打开）
	valveIO_t valve_type;	//valve type
}MagneticValveData;

static void init(MagneticValve * me, GPIO_TypeDef * port, u16 pin, u8 onLevel);
static void setState(MagneticValve * me, u8 state);
static u8 getState(MagneticValve * me);

static MagneticValve magneticValveData[MAX_NUM_V];
static MagneticValve magneticValve[MAX_NUM_V];

MagneticValve * MagneticValve_create(valveIO_t valve_type)
{
	MagneticValve * me = &magneticValveData[valve_type];
	MagneticValveData * pMagData = 0;
	if(me->init == 0)
	{
		me->init = init;
		me->setState = setState;
		me->getState = getState;
		me->private_data = &magneticValve[valve_type];
		pMagData = (MagneticValveData *)me->private_data;
		pMagData->valve_type = valve_type;
	
	}
	return me;
}

static void init(MagneticValve * me, GPIO_TypeDef * port, u16 pin, u8 onLevel)
{
	MagneticValveData * privateData = (MagneticValveData *)me->private_data;
	GPIO_InitTypeDef GPIO_Initure;
	static u8 initFlag = 0;
	if(initFlag == 0)
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			//开启GPIOA时钟
		__HAL_RCC_GPIOB_CLK_ENABLE();			//开启GPIOB时钟
		__HAL_RCC_GPIOC_CLK_ENABLE();			//开启GPIOC时钟
		__HAL_RCC_GPIOD_CLK_ENABLE();			//开启GPIOD时钟
		__HAL_RCC_GPIOE_CLK_ENABLE();			//开启GPIOE时钟
		__HAL_RCC_GPIOG_CLK_ENABLE();			//开启GPIOG时钟
		__HAL_RCC_GPIOH_CLK_ENABLE();			//开启GPIOH时钟
		initFlag = 1;
	}
	
	privateData->port = port;
	privateData->pin = pin;
	privateData->onLevel = onLevel;
	
	/* IO初始化 */
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;	//推挽输出
	GPIO_Initure.Pull = GPIO_PULLUP;		//上拉
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;	//高速
	GPIO_Initure.Pin = privateData->pin;
	HAL_GPIO_Init(privateData->port, &GPIO_Initure);
	
	me->setState(me, 0);
}

static void setState(MagneticValve * me, u8 state)
{
	MagneticValveData * privateData = (MagneticValveData *)me->private_data;
	privateData->preState = privateData->curState;
	privateData->curState = state;
	if(state == 0)
	{
		if(privateData->onLevel == 1)
			HAL_GPIO_WritePin(privateData->port, privateData->pin, GPIO_PIN_RESET);
		else
			HAL_GPIO_WritePin(privateData->port, privateData->pin, GPIO_PIN_SET);
	}
	else
	{
		if(privateData->onLevel == 1)
			HAL_GPIO_WritePin(privateData->port, privateData->pin, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(privateData->port, privateData->pin, GPIO_PIN_RESET);
	}
}

static u8 getState(MagneticValve * me)
{
	MagneticValveData * privateData = (MagneticValveData *)me->private_data;
	return privateData->curState;
}


