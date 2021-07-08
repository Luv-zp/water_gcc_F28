#include "BLL_Modbus_Server.h"
#include "mb.h"
#include "mbport.h"
#include "lwip_comm.h"
#include "lwip_client_app.h"
#include "lwip_server_app.h"
#include "FML_DelayTime.h"
#include "FML_ExtValve.h"
#include "FML_ExtSignal.h"
#include "FML_ExtProporV.h"
#include "userApp.h"
#include "BLL_MainFsm.h"



/*------------------------Slave mode use these variables----------------------*/
////Slave mode:DiscreteInputs variables
//USHORT   usDiscInStart = DISCRETE_INPUT_START;
//#if DISCRETE_INPUT_NDISCRETES % 8
//UCHAR    ucDiscInBuf[DISCRETE_INPUT_NDISCRETES / 8 + 1];
//#else
//UCHAR    ucDiscInBuf[DISCRETE_INPUT_NDISCRETES / 8];
//#endif
////Slave mode:Coils variables
//USHORT   usCoilStart = COIL_START;
//#if COIL_NCOILS % 8
//UCHAR    ucCoilBuf[COIL_NCOILS / 8 + 1];
//#else
//UCHAR    ucCoilBuf[COIL_NCOILS / 8];
//#endif
////Slave mode:InputRegister variables
//USHORT   usRegInStart = REG_INPUT_START;
//USHORT   usRegInBuf[REG_INPUT_NREGS];
////Slave mode:HoldingRegister variables
//USHORT   usRegHoldStart = REG_HOLDING_START;
//USHORT   usRegHoldBuf[REG_HOLDING_NREGS];


USHORT   usDiscInStart = DISCRETE_INPUT_START;
UCHAR    ucDiscInBuf[DISCRETE_INPUT_NDISCRETES];
USHORT   usCoilStart = COIL_START;
UCHAR    ucCoilBuf[COIL_NCOILS];
USHORT   usRegInStart = REG_INPUT_START;
USHORT   usRegInBuf[REG_INPUT_NREGS];
USHORT   usRegHoldStart = REG_HOLDING_START;
USHORT   usRegHoldBuf[REG_HOLDING_NREGS];
//-----------------------------------------------------------------------------

static void Modbus_init(void);
static void Modbus_devRun(void);
static void update_ext_v_cb(void *, void *);
static void update_ext_sign_cb(void *, void *);
static void update_prop_v_cb(void *, void *);
static void update_delay_time(void *, void *);

ModbusDev modbusDev_object;
static u8 sendBuff[1000];
static u16 dataLen;

ModbusDev * ModbusDev_create(void)
{
	static ModbusDev * me = 0;
	if(me == 0)
	{
		me = &modbusDev_object;
		me->init = Modbus_init;
		me->devRun_thread = Modbus_devRun;
		
		me->pRecvBuff = 0;
		me->pSendBuff = sendBuff;
		me->init();
	}
	return me;
}

static void Modbus_init(void)
{
	eMBErrorCode eStatus;
	eStatus = eMBTCPInit(502);
	/* Enable the modbus protocol Stack */
	eStatus = eMBEnable();
	
	/* 注册回调函数 */
	DelayTimer * pDelayTimer = DelayTimer_create(D_TIM_1);
	pDelayTimer->registerCallback(pDelayTimer, &modbusDev_object, update_delay_time, TIME_DISP);
	
	ValveIODev * pValveIODev = ValveIODev_create();
	pValveIODev->registerCallback(pValveIODev, &modbusDev_object, update_ext_v_cb, CB_V_DISP);
	
	ExitSignal * pExitSignal = ExitSignal_create();
	pExitSignal->registerCallback(pExitSignal, &modbusDev_object, update_ext_sign_cb, EXT_SIGN_UPDATE);
	
	ProporValDev * pProporValDev = ProporValDev_create();
	pProporValDev->registerCallback(pProporValDev, &modbusDev_object, update_prop_v_cb);
}

static void Modbus_devRun(void)
{
	BaseType_t res;
	AppMessage appMsg;
	u32 * pU32Temp = 0;
	int i = 0;
	
	while(1)
	{
		res = xQueueReceive(modbusQ, &appMsg, portMAX_DELAY);
		if(res == pdTRUE)
		{
			switch(appMsg.dataType)
			{
				case MB_UPDATE_DELAY_TIME:
					usRegInBuf[MB_DELAY_TIME_HI] = ((*(u32 *)appMsg.pVoid) >> 16) & 0xFFFF;
					usRegInBuf[MB_DELAY_TIME_LO] = (*(u32 *)appMsg.pVoid) & 0xFFFF;
					break;
				case MB_UPDATE_STATE:
					usRegInBuf[MB_STATE] = (u32)appMsg.pVoid & 0xFFFF; //传过来的直接是指针
					break;
				case MB_UPDATE_WATER_PRES:
					//更新水压数据 
					pU32Temp = (u32 *)appMsg.pVoid;
					usRegInBuf[MB_WATER_PRES_HI] = (*pU32Temp >>16) & 0xFFFF;
					usRegInBuf[MB_WATER_PRES_LO] = (*pU32Temp) & 0xFFFF;
					break;
				case MB_UPDATE_ABS_PRES:
					//更新负压
					pU32Temp = (u32 *)appMsg.pVoid;
					usRegInBuf[MB_ABS_PRES_HI] = (*pU32Temp >>16) & 0xFFFF;
					usRegInBuf[MB_ABS_PRES_LO] = (*pU32Temp) & 0xFFFF;
					break;
				case MB_UPDATE_LOW_1:
					//更新#1低压泄露
					usRegInBuf[MB_L_LEAK_1_HI] = ((u32)appMsg.pVoid >> 16) & 0xFFFF;	//传过来的直接是指针
					usRegInBuf[MB_L_LEAK_1_LO] = (u32)appMsg.pVoid & 0xFFFF;
					break;
				case MB_UPDATE_LOW_2:
					//更新#2低压泄露
					usRegInBuf[MB_L_LEAK_2_HI] = ((u32)appMsg.pVoid >> 16) & 0xFFFF;	//传过来的直接是指针
					usRegInBuf[MB_L_LEAK_2_LO] = (u32)appMsg.pVoid & 0xFFFF;
					break;
				case MB_UPDATE_LOW_3:
					//更新#3低压泄露
					usRegInBuf[MB_L_LEAK_3_HI] = ((u32)appMsg.pVoid >> 16) & 0xFFFF;	//传过来的直接是指针
					usRegInBuf[MB_L_LEAK_3_LO] = (u32)appMsg.pVoid & 0xFFFF;
					break;
				case MB_UPDATE_LOW_4:
					//更新#4低压泄露
					usRegInBuf[MB_H_LEAK_4_HI] = ((u32)appMsg.pVoid >> 16) & 0xFFFF;	//传过来的直接是指针
					usRegInBuf[MB_H_LEAK_4_LO] = (u32)appMsg.pVoid & 0xFFFF;
					break;
				case MB_UPDATE_HIGH_1:
					//更新#1高压泄露
					usRegInBuf[MB_H_LEAK_1_HI] = ((u32)appMsg.pVoid >> 16) & 0xFFFF;	//传过来的直接是指针
					usRegInBuf[MB_H_LEAK_1_LO] = (u32)appMsg.pVoid & 0xFFFF;
					break;
				case MB_UPDATE_HIGH_2:
					//更新#2高压泄露
					usRegInBuf[MB_H_LEAK_2_HI] = ((u32)appMsg.pVoid >> 16) & 0xFFFF;	//传过来的直接是指针
					usRegInBuf[MB_H_LEAK_2_LO] = (u32)appMsg.pVoid & 0xFFFF;
					break;
				case MB_UPDATE_HIGH_3:
					//更新#3高压泄露
					usRegInBuf[MB_H_LEAK_3_HI] = ((u32)appMsg.pVoid >> 16) & 0xFFFF;	//传过来的直接是指针
					usRegInBuf[MB_H_LEAK_3_LO] = (u32)appMsg.pVoid & 0xFFFF;
					break;
				case MB_UPDATE_HIGH_4:
					//更新#4高压泄露
					usRegInBuf[MB_H_LEAK_4_HI] = ((u32)appMsg.pVoid >> 16) & 0xFFFF;	//传过来的直接是指针
					usRegInBuf[MB_H_LEAK_4_LO] = (u32)appMsg.pVoid & 0xFFFF;
					break;
				case MB_UPDATE_RES:
					//更新结果，接收到的消息为u32，最高位表示结果0-ok, 1-ng,1-8位表示不同类别的测试结果(具体看VB_RES_t)
					usRegInBuf[MB_RES_FLAG] = *(u16 *)appMsg.pVoid;
					break;
				case MB_UPDATE_ALL_RES:
					//更新结果，接收到的消息为u32，最低4位为#1，2,3,4的低压结果，次高4位为高压结果 0-ok, 1-ng,
					usRegInBuf[MB_ALL_RES] = *(u16 *)appMsg.pVoid;
					break;
				case MB_CLEAR_FLAG:
					usRegInBuf[MB_DATA_UPDATE_FLAG] = 0;
					break;
				case MB_UPDATE_ERR:
					//更新错误状态寄存器
					usRegInBuf[MB_ERROR] = *(u32 *)appMsg.pVoid & 0xFFFF;
					usRegInBuf[MB_ERR_2] = (*(u32 *)appMsg.pVoid >> 16) & 0xFFFF;
					break;
				case MB_UPDATE_DATA_FALG:
					//指示哪个数据正在更新, 传过来的是指针
					usRegInBuf[MB_DATA_UPDATE_FLAG] = (u16)appMsg.pVoid;
					break;
				case MB_TEST_OVER:
					//每次收到开始信号会发出消息，此时会将当前的测试数据复制给PRE数据进行保存，同时会把当前数据清0
					for(i = 0; i < 8; i ++)
					{
						usRegInBuf[33 + 2*i] = usRegInBuf[7 + 2*i];
						usRegInBuf[33 + 2*i + 1] = usRegInBuf[7 + 2*i + 1];
						usRegInBuf[7 + 2*i] = 0;
						usRegInBuf[7 + 2*i + 1] = 0;
					}
					break;
				case MB_UPDATE_VB_STATUS:
					usRegInBuf[MB_VB_STATUS] = *(u16 *)appMsg.pVoid;
					break;
				default:
					break;		
			}
		}
	}
}

//更新阀门状态回调函数
//pInstance 注册回调函数的对象
//pVoid 指向保存阀门状态的指针
static void update_ext_v_cb(void * pInstance, void * pVoid)
{
	int i = 0;
	for(i = 0; i< MAX_NUM_V; i++)
	{
		if(((u8 *)pVoid)[i])
			ucCoilBuf[i/8] |= (1 << (i%8));
		else
			ucCoilBuf[i/8] &= ~(1 << (i%8));
	}
}

//更新传感器状态回调函数
//pInstance 注册回调函数的对象
//pVoid 指向保存传感器状态的指针
static void update_ext_sign_cb(void * pInstance, void * pVoid)
{
	u8 sensorState = 0;
	u16 mbSensorState = 0;
	sensorState = *(u8 *)pVoid;
	if(sensorState & (1 << WATER_DETEC_SIGN)) mbSensorState |= MB_SENS_LIQ;
	if(sensorState & (1 << VACUUM_SIGN)) mbSensorState |= MB_SENS_VACU;
	if(sensorState & (1 << U_CYL_POS_SIGN)) mbSensorState |= MB_SENS_CYL_U;
	if(sensorState & (1 << D_CYL_POS_SIGN)) mbSensorState |= MB_SENS_CYL_D;
	if(sensorState & (1 << GRAT_SIGN)) mbSensorState |= MB_SENS_GRAT;
	usRegInBuf[MB_SENS_FLAG] = mbSensorState;
}

//更新比例阀开度回调函数
//pInstance 注册回调函数的对象
//pVoid 指向比例阀开度的指针
static void update_prop_v_cb(void * pInstance, void * pVoid)
{
	usRegHoldBuf[MB_OPENING_HI] = (*(u32 *)pVoid >> 16) & 0xFFFF;
	usRegHoldBuf[MB_OPENING_LO] = *(u32 *)pVoid & 0xFFFF;
}

//倒计时剩余时间显示更新
//pInstance 注册回调函数的对象
//pVoid 指向剩余时间的指针
static void update_delay_time(void * pInstance, void * pVoid)
{
	usRegInBuf[MB_DELAY_TIME_HI] = (*(u32 *)pVoid >> 16) & 0xFFFF;
	usRegInBuf[MB_DELAY_TIME_LO] = *(u32 *)pVoid & 0xFFFF;
}


/* ----------------------- TCP port functions -------------------------------*/
BOOL            xMBTCPPortInit( USHORT usTCPPort )
{
	lwip_comm_init();	//以太网网口初始化
	tcp_server_init();
	return TRUE;
}

void            vMBTCPPortClose( void )
{
}

void            vMBTCPPortDisable( void )
{
}

BOOL            xMBTCPPortGetRequest( UCHAR **ppucMBTCPFrame, USHORT * usTCPLength )
{
	* ppucMBTCPFrame = modbusDev_object.pRecvBuff;
	* usTCPLength = dataLen;
	return TRUE;
}

BOOL            xMBTCPPortSendResponse( const UCHAR *pucMBTCPFrame, USHORT usTCPLength )
{
	AppMessage appMsg;
	appMsg.dataType = usTCPLength;
	appMsg.pVoid = (void *)pucMBTCPFrame;
	xQueueSend(server_netQ, &appMsg, 10);
	return TRUE;
}


/**
  *****************************************************************************
  * @Name   : 操作输入寄存器
  *
  * @Brief  : 对应功能码0x04 -> eMBFuncReadInputRegister
  *
  * @Input  : *pucRegBuffer：数据缓冲区，响应主机用
  *           usAddress:     寄存器地址
  *           usNRegs:       操作寄存器个数
  *
  * @Output : none
  *
  * @Return : Modbus状态信息
  *****************************************************************************
**/
eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
	eMBErrorCode eStatus = MB_ENOERR;
	int          iRegIndex = 0;
	
	/* it already plus one in modbus function method. */
//    usAddress--;
	
	//判断地址合法性
	if ((usAddress >= REG_INPUT_START) && (usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS))
	{
		iRegIndex = (int)(usAddress - usRegInStart);
		while (usNRegs > 0)
		{
			*pucRegBuffer++ = (UCHAR)( usRegInBuf[iRegIndex] >> 8);  //高8位字节
			*pucRegBuffer++ = (UCHAR)( usRegInBuf[iRegIndex] & 0xFF); //低8位字节
			iRegIndex++;
			usNRegs--;
		}
	}
	else  //错误地址
	{
		eStatus = MB_ENOREG;
	}
	return eStatus;
}

/**
  *****************************************************************************
  * @Name   : 操作保持寄存器
  *
  * @Brief  : 对应功能码0x06 -> eMBFuncWriteHoldingRegister
  *                    0x16 -> eMBFuncWriteMultipleHoldingRegister
  *                    0x03 -> eMBFuncReadHoldingRegister
  *                    0x23 -> eMBFuncReadWriteMultipleHoldingRegister
  *
  * @Input  : *pucRegBuffer：数据缓冲区，响应主机用
  *           usAddress:     寄存器地址
  *           usNRegs:       操作寄存器个数
  *           eMode:         功能码
  *
  * @Output : none
  *
  * @Return : Modbus状态信息
  *****************************************************************************
**/
eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
	eMBErrorCode eStatus = MB_ENOERR;
	int          iRegIndex = 0;
	
	//
	AppMessage appMsg;
	FiniteStateMachine * me = FiniteStateMachine_create();
	u16 iNRegs = usNRegs;
	
	/* it already plus one in modbus function method. */
//    usAddress--;
	
	//
	//判断地址是否合法
	if((usAddress >= REG_HOLDING_START) && ((usAddress + usNRegs) <= (REG_HOLDING_START + REG_HOLDING_NREGS)))
	{
		iRegIndex = (int)(usAddress - usRegHoldStart);	//得到偏移量
		//
		//根据功能码进行操作
		//
		switch(eMode)
		{
			case MB_REG_READ:  //读保持寄存器
					while(usNRegs > 0)
					{
						*pucRegBuffer++ = (uint8_t)(usRegHoldBuf[iRegIndex] >> 8);  //高8位字节
						*pucRegBuffer++ = (uint8_t)(usRegHoldBuf[iRegIndex] & 0xFF); //低8位字节
						iRegIndex++;
						usNRegs--;
					}                            
					break;
					
			case MB_REG_WRITE:  //写保持寄存器
					while(usNRegs > 0)
					{
						usRegHoldBuf[iRegIndex] = *pucRegBuffer++ << 8;  //高8位字节
						usRegHoldBuf[iRegIndex] |= *pucRegBuffer++;  //低8位字节
						iRegIndex++;
						usNRegs--;
					}
					//更新参数
					if(((usAddress - usRegHoldStart) == 0) && (iNRegs == 84))
					{
						//收到了更新参数的数据，将参数数组首地址的指针发给设备控制线程
						appMsg.dataType = EV_SET_PARA;
						appMsg.pVoid = usRegHoldBuf;
						me->sendEvent(&appMsg);
					}
					else if(((usAddress - usRegHoldStart) == 98) && (iNRegs == 2))
					{
						//此时收到的数据为电子比例阀开度，仅仅在DEBUG模式下有效
						appMsg.dataType = EV_SET_PROPOR;
						appMsg.pVoid = &usRegHoldBuf[98];
						me->sendEvent(&appMsg);
					}
					break;
		}
	}
	else  //错误地址
	{
		eStatus = MB_ENOREG;
	}
	return eStatus;
}

/**
  *****************************************************************************
  * @Name   : 操作线圈
  *
  * @Brief  : 对应功能码0x01 -> eMBFuncReadCoils
  *                    0x05 -> eMBFuncWriteCoil
  *                    0x15 -> 写多个线圈 eMBFuncWriteMultipleCoils
  *
  * @Input  : *pucRegBuffer：数据缓冲区，响应主机用
  *           usAddress:     寄存器地址
  *           usNCoils:      操作线圈个数
  *           eMode:         功能码
  *
  * @Output : none
  *
  * @Return : Modbus状态信息
  *****************************************************************************
**/
eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
	eMBErrorCode eStatus = MB_ENOERR;
	short iNCoils = (short)usNCoils;
	unsigned short usBitOffset = 0;
	
	//
	AppMessage appMsg;
	FiniteStateMachine * me = FiniteStateMachine_create();

	/* it already plus one in modbus function method. */
//    usAddress--;
	
	//
	//判断地址合法性
	if ((usAddress >= COIL_START) && ((usAddress + usNCoils) <= (COIL_START + COIL_NCOILS)))
	{
		usBitOffset = usAddress - COIL_START;
		//
		//根据功能码操作
		//
		switch (eMode)
		{
			case MB_REG_READ:  //读取操作
				while (iNCoils > 0)
				{
					*pucRegBuffer++ = xMBUtilGetBits(ucCoilBuf, usBitOffset, (u8)(iNCoils > 8 ? 8: iNCoils));
					iNCoils -= 8;
					usBitOffset += 8;
				}
				break;
			case MB_REG_WRITE:  //写操作
				while (iNCoils > 0)
				{
					xMBUtilSetBits(ucCoilBuf, usBitOffset, (u8)(iNCoils > 8 ? 8: iNCoils), *pucRegBuffer++);
					iNCoils -= 8;	
				}
				/*--------------------------------根据线圈不同状态执行不同命令----------------------------------*/
				if((usBitOffset == MB_START) && (usNCoils == 1))
				{
					if(ucCoilBuf[MB_START / 8] & (1 << (MB_START % 8)))
					{
						ucCoilBuf[MB_STOP / 8] &= ~(1 << (MB_STOP % 8));
						appMsg.dataType = EV_START;
						me->sendEvent(&appMsg);
					}
				}
				else if((usBitOffset == MB_STOP) && (usNCoils == 1))
				{
					if(ucCoilBuf[MB_STOP / 8] & (1 << (MB_STOP % 8)))
					{
						ucCoilBuf[MB_START / 8] &= ~(1 << (MB_START % 8));
						appMsg.dataType = EV_STOP;
						me->sendEvent(&appMsg);
					}
				}
				else if((usBitOffset >= MB_WATER_IN_V) && (usBitOffset <= MB_CYL_D_V) && (usNCoils == 1))
				{
					//单独控制阀门的时候，根据阀门编号来给设备控制线程发送阀门控制信息
					if(ucCoilBuf[usBitOffset / 8] & (1 << (usBitOffset % 8)))
						appMsg.pVoid = (void *)(0x80000000 | (1 << usBitOffset));	//直接发送指针
					else
						appMsg.pVoid = (void *)(0x7FFFFFFF | (1 << usBitOffset));
					appMsg.dataType = EV_SET_SIG_V;
					me->sendEvent(&appMsg);
				}
				else if((usBitOffset == MB_DEBUG) && (usNCoils == 1))
				{
					appMsg.dataType = EV_DEBUG_MODE;
					me->sendEvent(&appMsg);
				}
				else if((usBitOffset == MB_DRY) && (usNCoils == 1))
				{
					appMsg.dataType = EV_DRY_MODE;
					me->sendEvent(&appMsg);
				}
				//---------------------------------------------------------------------------------------
				break;
			default:
				break;
		}
	}
	else  //错误地址
	{
		eStatus = MB_ENOREG;
	}
	
	return eStatus;
}

/**
  *****************************************************************************
  * @Name   : 操作离散寄存器
  *
  * @Brief  : 对应功能码0x02 -> eMBFuncReadDiscreteInputs
  *
  * @Input  : *pucRegBuffer：数据缓冲区，响应主机用
  *           usAddress:     寄存器地址
  *           usNDiscrete:   操作寄存器个数
  *
  * @Output : none
  *
  * @Return : Modbus状态信息
  *****************************************************************************
**/
eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
	eMBErrorCode eStatus = MB_ENOERR;
	short iNDiscrete = (short)usNDiscrete;
	u16 usBitOffset = 0;
	
	//
	//判断地址合法性
	if ((usAddress >= DISCRETE_INPUT_START) && ((usAddress + usNDiscrete) <= (DISCRETE_INPUT_START + DISCRETE_INPUT_NDISCRETES)))
	{
		usBitOffset = usAddress - DISCRETE_INPUT_START;
		while(iNDiscrete > 0)
		{
			*pucRegBuffer++ = xMBUtilGetBits(ucCoilBuf, usBitOffset, (u8)(iNDiscrete > 8 ? 8: iNDiscrete));
			iNDiscrete -= 8;
			usBitOffset += 8;
		}
	}
	else  //错误地址
	{
		eStatus = MB_ENOREG;
	}
	
	return eStatus;
}

