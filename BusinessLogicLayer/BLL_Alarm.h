#ifndef _BLL_ALARM_H
#define _BLL_ALARM_H
#include "sys.h"


//设置一个不满足测试条件的标志位
extern u8 testCondition;

/* 错误代码类型枚举 */
typedef enum 
{
	ERR_VACUUM				= 0x0001,	//真空度不足
	ERR_NO_PRODUCT			= 0x0002,	//产品检测错误
	ERR_CLEAN				= 0x0004,	//自洁错误
	ERR_WATER_PRES			= 0x0008,	//水压故障
	ERR_WATER_IN_TIME_OUT	= 0x0010,	//注水超时
	ERR_L_PRES				= 0x0020,	
	ERR_ADD_PRES_TIME_OUT	= 0x0040,	//加压超时
	ERR_H_PRES				= 0x0080,
	ERR_GRAT				= 0x0100,	//光栅报警
	ERR_PUMP				= 0x0200,	//真空泵故障
	ERR_CYL_D				= 0x0400,	//气缸下降未到位
	ERR_CYL_U				= 0x0800,	//气缸上抬未到位
	ERR_LIQ					= 0x1000,	//液位故障
	ERR_REVERSE_1			= 0x2000,	
	ERR_REVERSE_2			= 0x4000,
	ERR_NEED_DRY			= 0x8000,	//需要干燥
	ERR_DRY					= 0x10000,	//干燥错误
	ERR_CYL_F				= 0x20000,
	ERR_CYL_B				= 0x40000
}ERROR_t;

/* VB1401状态类型枚举 */
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

/* 结果类型枚举 */
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

/* 标志位类型枚举 */
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

