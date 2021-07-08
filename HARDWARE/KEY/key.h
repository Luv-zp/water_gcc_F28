#ifndef _KEY_H
#define _KEY_H
#include "sys.h"

/*k�����ʹ��������Ŷ���*/
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

/*�ⲿ�������ź�������*/
typedef enum
{
	WATER_DETEC_SIGN = 0,	//Һλ����
	VACUUM_SIGN,			//��ձ�
	AIR_PRES_SIGN,			//ѹ����
	U_CYL_POS_SIGN,			//�����ϵ�λ
	D_CYL_POS_SIGN,			//�����µ�λ
	F_CYL_POS_SIGN,			//����ǰ��λ
	B_CYL_POS_SIGN,			//���׺�λ
	GRAT_SIGN,				//��դ
	SIGN_NUM_MAX			//�ź�������
}ExitSign_t;

//������������ʽ
typedef enum
{
	LOW_LEVEL_TRIG = 0,		//�͵�ƽ����
	HIGH_LEVEL_TRIG			//�ߵ�ƽ����
}VolTrig_t;

//������״̬
typedef enum
{
	TRIG_OFF, 	//������δ����
	TRIG_ON,	//����
}SwitchSensorState_t;

//���ش�������
typedef struct _SwitchSensor SwitchSensor;
struct _SwitchSensor
{
	void (* init)(SwitchSensor * me);
	u8 (* signDetect)(SwitchSensor * me, SwitchSensorState_t * state); //�źż��
	void (* setTrigType)(SwitchSensor * me, VolTrig_t type); //���ô�����ƽ
	void (* setTrigCountLimit)(SwitchSensor * me, u32 trig_on, u32 trig_off);
	
	void * private_data; //˽������
};

SwitchSensor * SwitchSensor_create(ExitSign_t type);

#endif
