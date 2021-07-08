#ifndef _BLL_ALARM_H
#define _BLL_ALARM_H
#include "sys.h"


//����һ����������������ı�־λ
extern u8 testCondition;

/* �����������ö�� */
typedef enum 
{
	ERR_VACUUM				= 0x0001,	//��նȲ���
	ERR_NO_PRODUCT			= 0x0002,	//��Ʒ������
	ERR_CLEAN				= 0x0004,	//�Խ����
	ERR_WATER_PRES			= 0x0008,	//ˮѹ����
	ERR_WATER_IN_TIME_OUT	= 0x0010,	//עˮ��ʱ
	ERR_L_PRES				= 0x0020,	
	ERR_ADD_PRES_TIME_OUT	= 0x0040,	//��ѹ��ʱ
	ERR_H_PRES				= 0x0080,
	ERR_GRAT				= 0x0100,	//��դ����
	ERR_PUMP				= 0x0200,	//��ձù���
	ERR_CYL_D				= 0x0400,	//�����½�δ��λ
	ERR_CYL_U				= 0x0800,	//������̧δ��λ
	ERR_LIQ					= 0x1000,	//Һλ����
	ERR_REVERSE_1			= 0x2000,	
	ERR_REVERSE_2			= 0x4000,
	ERR_NEED_DRY			= 0x8000,	//��Ҫ����
	ERR_DRY					= 0x10000,	//�������
	ERR_CYL_F				= 0x20000,
	ERR_CYL_B				= 0x40000
}ERROR_t;

/* VB1401״̬����ö�� */
typedef enum
{
	EX_READY_S			= 0x01,
	EX_RUN_S			= 0x02,
	EX_TEST_S			= 0x04,
	EX_ERR_S			= 0x08,
	EX_END_S			= 0x10,
	RESERVE				= 0x20,
	EX_DEBUG_S			= 0x40,
	EX_DRY_S			= 0x80,
}VB_STATUS_t;

/* �������ö�� */
typedef enum
{
	EX_OK1_S			= 0x01,
	EX_NG1_S			= 0x02,
	EX_OK2_S			= 0x04,
	EX_NG2_S			= 0x08,
	EX_OK3_S			= 0x10,
	EX_NG3_S			= 0x20,
	EX_OK4_S			= 0x40,
	EX_NG4_S			= 0x80,
}VB_RES_t;

/* ��־λ����ö�� */
typedef enum
{
	FLAG_ERROR = 0,
	FLAG_VB_STATUS,
	FLAG_VB_RES
}FLAG_TYPE_t;

typedef struct _FlagHand FlagHand;
struct _FlagHand
{
	void (* init)(void);	
	
	void (* setErrBit)(ERROR_t type);			
	void (* clearErrBit)(ERROR_t type);
	u32 (* getAllErr)(void);
	void (* clearAllErr)(void);
	
	void (* setVbStatus)(VB_STATUS_t type);
	void (* clearVbStatus)(VB_STATUS_t type);
	u16 (* getAllVbStatus)(void);
	void (* clearAllVbStatus)(void);
	
	void (* setVbRes)(VB_RES_t type);
	void (* clearVbRes)(VB_RES_t type);
	u16 (* getAllVbRes)(void);
	void (* clearAllVbRes)(void);
	
	void (* threadRun)(void);
};

FlagHand * errHand_create(void);

#endif

