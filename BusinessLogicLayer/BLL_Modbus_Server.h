#ifndef _BLL_MODBUS_SERVER_H
#define _BLL_MODBUS_SERVER_H
#include "sys.h"
#include "port.h"


/******************************************************************************
                               ����Modbus��ز���
******************************************************************************/
/* -----------------------Slave Defines -------------------------------------*/
#define DISCRETE_INPUT_START        1
#define DISCRETE_INPUT_NDISCRETES   100
#define COIL_START                  101
#define COIL_NCOILS                 100
#define REG_INPUT_START             201
#define REG_INPUT_NREGS             100
#define REG_HOLDING_START           301
#define REG_HOLDING_NREGS           100
//------------------------------------------------------------------------------

/*-------------------------- ״̬�������ͺ궨��--------------------------- */
#define MB_UPDATE_WATER_PRES	1
#define MB_UPDATE_ABS_PRES		2
#define MB_UPDATE_STATE			3
#define MB_UPDATE_DELAY_TIME	4
#define MB_UPDATE_LOW_1			5
#define MB_UPDATE_LOW_2			6
#define MB_UPDATE_LOW_3			7
#define MB_UPDATE_LOW_4			8
#define MB_UPDATE_HIGH_1		9
#define MB_UPDATE_HIGH_2		10
#define MB_UPDATE_HIGH_3		11
#define MB_UPDATE_HIGH_4		12
#define MB_UPDATE_VALVE			13
#define MB_UPDATE_SENS			14
#define MB_UPDATE_RES			15
#define MB_CLEAR_FLAG			16
#define MB_UPDATE_ERR			17
#define MB_UPDATE_DATA_FALG		18
#define MB_UPDATE_SENS_FLAG		19
#define MB_TEST_OVER			20
#define MB_UPDATE_OPENING		21
#define MB_UPDATE_VB_STATUS		22
#define MB_UPDATE_ALL_RES		23
//--------------------------------------------------------------------------

/*----------------------------------�Ĵ���λ�ú궨��------------------------------- */
//ֻ���Ĵ���ƫ����
#define MB_WATER_PRES_HI		0
#define MB_WATER_PRES_LO		1
#define MB_ABS_PRES_HI			2
#define MB_ABS_PRES_LO			3
#define MB_STATE				4
#define MB_DELAY_TIME_HI		5
#define MB_DELAY_TIME_LO		6
#define MB_L_LEAK_1_HI			7
#define MB_L_LEAK_1_LO			8
#define MB_L_LEAK_2_HI			9
#define MB_L_LEAK_2_LO			10
#define MB_L_LEAK_3_HI			11
#define MB_L_LEAK_3_LO			12
#define MB_L_LEAK_4_HI			13
#define MB_L_LEAK_4_LO			14
#define MB_H_LEAK_1_HI			15
#define MB_H_LEAK_1_LO			16
#define MB_H_LEAK_2_HI			17
#define MB_H_LEAK_2_LO			18
#define MB_H_LEAK_3_HI			19
#define MB_H_LEAK_3_LO			20
#define MB_H_LEAK_4_HI			21
#define MB_H_LEAK_4_LO			22

#define MB_ERR_2				27
#define MB_VB_STATUS			28
#define MB_RES_FLAG				29
#define MB_DATA_UPDATE_FLAG		30
#define MB_ERROR				31
#define MB_SENS_FLAG			32

#define MB_L_LEAK_1_PRE_HI		33
#define MB_L_LEAK_1_PRE_LO		34
#define MB_L_LEAK_2_PRE_HI		35
#define MB_L_LEAK_2_PRE_LO		36
#define MB_L_LEAK_3_PRE_HI		37
#define MB_L_LEAK_3_PRE_LO		38
#define MB_L_LEAK_4_PRE_HI		39
#define MB_L_LEAK_4_PRE_LO		40
#define MB_H_LEAK_1_PRE_HI		41
#define MB_H_LEAK_1_PRE_LO		42
#define MB_H_LEAK_2_PRE_HI		43
#define MB_H_LEAK_2_PRE_LO		44
#define MB_H_LEAK_3_PRE_HI		45
#define MB_H_LEAK_3_PRE_LO		46
#define MB_H_LEAK_4_PRE_HI		47
#define MB_H_LEAK_4_PRE_LO		48
#define MB_ALL_RES				49
//---------------

//���ּĴ���ƫ����
#define MB_OPENING_HI		98
#define MB_OPENING_LO		99
//---------------

//��д��Ȧƫ����
#define MB_WATER_IN_V		0
#define MB_ADD_PRES_V		1
#define MB_PUMP_WATER_V		2
#define MB_AIR_IN_V			3
#define MB_WATER_OUT_V		4
#define MB_DRY_V			5
#define MB_H_VACUUM_V		6
#define MB_H_LEAK_1_V		7
#define MB_H_LEAK_2_V		8
#define MB_H_LEAK_3_V		9
#define MB_H_LEAK_4_V		10
#define MB_L_VACUUM_V		11
#define MB_L_LEAK_1_V		12
#define MB_L_LEAK_2_V		13
#define MB_L_LEAK_3_V		14
#define MB_L_LEAK_4_V		15
#define MB_AIR_OUT_V		16
#define MB_NEEDLE_V			17
#define MB_BALANCE_V		18
#define MB_WATER_PUMP_V		19
#define MB_CYL_U_V			20
#define MB_CYL_D_V			21

#define MB_START			32
#define MB_STOP				33
#define MB_DEBUG			34
#define MB_DRY				35
//-------------
//----------------------------------------------------------------------------------

/* ���ݸ��±�־λ */
#define L_LEAK_1_DATA		0x01
#define L_LEAK_2_DATA		0x02
#define L_LEAK_3_DATA		0x04
#define L_LEAK_4_DATA		0x08
#define H_LEAK_1_DATA		0x10
#define H_LEAK_2_DATA		0x20
#define H_LEAK_3_DATA		0x40
#define H_LEAK_4_DATA		0x80
#define DETEC_FINISH		0x4000
#define DETEC_STOP			0x8000

/* ���͸�MODBUS�Ĵ������豸״̬���� */
#define MB_STATE_READY			1
#define MB_STATE_CYL_DOWN		2
#define MB_STATE_13TOR			3
#define MB_STATE_EXIST			4
#define MB_STATE_PURGE			5
#define MB_STATE_PUMP			6
#define MB_STATE_IS_DRY			7
#define MB_STATE_WATER_IN		8
//
#define MB_STATE_L_PUMP			9
#define MB_STATE_L_BAL			10
#define MB_STATE_L_1_LEAK		11
#define MB_STATE_L_1_DRY		12
#define MB_STATE_L_2_LEAK		13
#define MB_STATE_L_2_DRY		14
#define MB_STATE_L_3_LEAK		15
#define MB_STATE_L_3_DRY		16
#define MB_STATE_L_4_LEAK		17
#define MB_STATE_ADD_PRES		18
#define MB_STATE_H_PUMP			19
#define MB_STATE_H_BAL			20
#define MB_STATE_H_1_LEAK		21
#define MB_STATE_H_1_DRY		22
#define MB_STATE_H_2_LEAK		23
#define MB_STATE_H_2_DRY		24
#define MB_STATE_H_3_LEAK		25
#define MB_STATE_H_3_DRY		26
#define MB_STATE_H_4_LEAK		27
//
#define MB_STATE_WATER_OUT		28
#define MB_STATE_WATER_PUMP		29
#define MB_STATE_CYL_UP			30
#define MB_STATE_DEBUG				31
#define MB_STATE_DRY_CYL_DOWN		32
#define MB_STATE_DRY_CYL_UP			33
#define MB_STATE_DRY_AIR_PURGE		34
#define MB_STATE_DRY_AIR_PUMP		35
#define MB_STATE_TEST_CONDITION		36

/* ������״̬ */
#define MB_SENS_LIQ			0x0001
#define MB_SENS_GRAT		0x0002
#define MB_SENS_VACU		0x0004
#define MB_SENS_DRY			0x0008
#define MB_SENS_CYL_U		0x0010
#define MB_SENS_CYL_D		0x0020



typedef struct _ModbusDev ModbusDev;
struct _ModbusDev
{
	void (* init)(void);
	void (* devRun_thread)(void);	//modbus��Ϣ����
	
	u8 *pRecvBuff;	//������յ�����
	u8 *pSendBuff;	//�������ݻ�����
};
ModbusDev * ModbusDev_create(void);



#endif

