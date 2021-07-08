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
	
	/* ע��ص����� */
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
					usRegInBuf[MB_STATE] = (u32)appMsg.pVoid & 0xFFFF; //��������ֱ����ָ��
					break;
				case MB_UPDATE_WATER_PRES:
					//����ˮѹ���� 
					pU32Temp = (u32 *)appMsg.pVoid;
					usRegInBuf[MB_WATER_PRES_HI] = (*pU32Temp >>16) & 0xFFFF;
					usRegInBuf[MB_WATER_PRES_LO] = (*pU32Temp) & 0xFFFF;
					break;
				case MB_UPDATE_ABS_PRES:
					//���¸�ѹ
					pU32Temp = (u32 *)appMsg.pVoid;
					usRegInBuf[MB_ABS_PRES_HI] = (*pU32Temp >>16) & 0xFFFF;
					usRegInBuf[MB_ABS_PRES_LO] = (*pU32Temp) & 0xFFFF;
					break;
				case MB_UPDATE_LOW_1:
					//����#1��ѹй¶
					usRegInBuf[MB_L_LEAK_1_HI] = ((u32)appMsg.pVoid >> 16) & 0xFFFF;	//��������ֱ����ָ��
					usRegInBuf[MB_L_LEAK_1_LO] = (u32)appMsg.pVoid & 0xFFFF;
					break;
				case MB_UPDATE_LOW_2:
					//����#2��ѹй¶
					usRegInBuf[MB_L_LEAK_2_HI] = ((u32)appMsg.pVoid >> 16) & 0xFFFF;	//��������ֱ����ָ��
					usRegInBuf[MB_L_LEAK_2_LO] = (u32)appMsg.pVoid & 0xFFFF;
					break;
				case MB_UPDATE_LOW_3:
					//����#3��ѹй¶
					usRegInBuf[MB_L_LEAK_3_HI] = ((u32)appMsg.pVoid >> 16) & 0xFFFF;	//��������ֱ����ָ��
					usRegInBuf[MB_L_LEAK_3_LO] = (u32)appMsg.pVoid & 0xFFFF;
					break;
				case MB_UPDATE_LOW_4:
					//����#4��ѹй¶
					usRegInBuf[MB_H_LEAK_4_HI] = ((u32)appMsg.pVoid >> 16) & 0xFFFF;	//��������ֱ����ָ��
					usRegInBuf[MB_H_LEAK_4_LO] = (u32)appMsg.pVoid & 0xFFFF;
					break;
				case MB_UPDATE_HIGH_1:
					//����#1��ѹй¶
					usRegInBuf[MB_H_LEAK_1_HI] = ((u32)appMsg.pVoid >> 16) & 0xFFFF;	//��������ֱ����ָ��
					usRegInBuf[MB_H_LEAK_1_LO] = (u32)appMsg.pVoid & 0xFFFF;
					break;
				case MB_UPDATE_HIGH_2:
					//����#2��ѹй¶
					usRegInBuf[MB_H_LEAK_2_HI] = ((u32)appMsg.pVoid >> 16) & 0xFFFF;	//��������ֱ����ָ��
					usRegInBuf[MB_H_LEAK_2_LO] = (u32)appMsg.pVoid & 0xFFFF;
					break;
				case MB_UPDATE_HIGH_3:
					//����#3��ѹй¶
					usRegInBuf[MB_H_LEAK_3_HI] = ((u32)appMsg.pVoid >> 16) & 0xFFFF;	//��������ֱ����ָ��
					usRegInBuf[MB_H_LEAK_3_LO] = (u32)appMsg.pVoid & 0xFFFF;
					break;
				case MB_UPDATE_HIGH_4:
					//����#4��ѹй¶
					usRegInBuf[MB_H_LEAK_4_HI] = ((u32)appMsg.pVoid >> 16) & 0xFFFF;	//��������ֱ����ָ��
					usRegInBuf[MB_H_LEAK_4_LO] = (u32)appMsg.pVoid & 0xFFFF;
					break;
				case MB_UPDATE_RES:
					//���½�������յ�����ϢΪu32�����λ��ʾ���0-ok, 1-ng,1-8λ��ʾ��ͬ���Ĳ��Խ��(���忴VB_RES_t)
					usRegInBuf[MB_RES_FLAG] = *(u16 *)appMsg.pVoid;
					break;
				case MB_UPDATE_ALL_RES:
					//���½�������յ�����ϢΪu32�����4λΪ#1��2,3,4�ĵ�ѹ������θ�4λΪ��ѹ��� 0-ok, 1-ng,
					usRegInBuf[MB_ALL_RES] = *(u16 *)appMsg.pVoid;
					break;
				case MB_CLEAR_FLAG:
					usRegInBuf[MB_DATA_UPDATE_FLAG] = 0;
					break;
				case MB_UPDATE_ERR:
					//���´���״̬�Ĵ���
					usRegInBuf[MB_ERROR] = *(u32 *)appMsg.pVoid & 0xFFFF;
					usRegInBuf[MB_ERR_2] = (*(u32 *)appMsg.pVoid >> 16) & 0xFFFF;
					break;
				case MB_UPDATE_DATA_FALG:
					//ָʾ�ĸ��������ڸ���, ����������ָ��
					usRegInBuf[MB_DATA_UPDATE_FLAG] = (u16)appMsg.pVoid;
					break;
				case MB_TEST_OVER:
					//ÿ���յ���ʼ�źŻᷢ����Ϣ����ʱ�Ὣ��ǰ�Ĳ������ݸ��Ƹ�PRE���ݽ��б��棬ͬʱ��ѵ�ǰ������0
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

//���·���״̬�ص�����
//pInstance ע��ص������Ķ���
//pVoid ָ�򱣴淧��״̬��ָ��
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

//���´�����״̬�ص�����
//pInstance ע��ص������Ķ���
//pVoid ָ�򱣴洫����״̬��ָ��
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

//���±��������Ȼص�����
//pInstance ע��ص������Ķ���
//pVoid ָ����������ȵ�ָ��
static void update_prop_v_cb(void * pInstance, void * pVoid)
{
	usRegHoldBuf[MB_OPENING_HI] = (*(u32 *)pVoid >> 16) & 0xFFFF;
	usRegHoldBuf[MB_OPENING_LO] = *(u32 *)pVoid & 0xFFFF;
}

//����ʱʣ��ʱ����ʾ����
//pInstance ע��ص������Ķ���
//pVoid ָ��ʣ��ʱ���ָ��
static void update_delay_time(void * pInstance, void * pVoid)
{
	usRegInBuf[MB_DELAY_TIME_HI] = (*(u32 *)pVoid >> 16) & 0xFFFF;
	usRegInBuf[MB_DELAY_TIME_LO] = *(u32 *)pVoid & 0xFFFF;
}


/* ----------------------- TCP port functions -------------------------------*/
BOOL            xMBTCPPortInit( USHORT usTCPPort )
{
	lwip_comm_init();	//��̫�����ڳ�ʼ��
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
  * @Name   : ��������Ĵ���
  *
  * @Brief  : ��Ӧ������0x04 -> eMBFuncReadInputRegister
  *
  * @Input  : *pucRegBuffer�����ݻ���������Ӧ������
  *           usAddress:     �Ĵ�����ַ
  *           usNRegs:       �����Ĵ�������
  *
  * @Output : none
  *
  * @Return : Modbus״̬��Ϣ
  *****************************************************************************
**/
eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
	eMBErrorCode eStatus = MB_ENOERR;
	int          iRegIndex = 0;
	
	/* it already plus one in modbus function method. */
//    usAddress--;
	
	//�жϵ�ַ�Ϸ���
	if ((usAddress >= REG_INPUT_START) && (usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS))
	{
		iRegIndex = (int)(usAddress - usRegInStart);
		while (usNRegs > 0)
		{
			*pucRegBuffer++ = (UCHAR)( usRegInBuf[iRegIndex] >> 8);  //��8λ�ֽ�
			*pucRegBuffer++ = (UCHAR)( usRegInBuf[iRegIndex] & 0xFF); //��8λ�ֽ�
			iRegIndex++;
			usNRegs--;
		}
	}
	else  //�����ַ
	{
		eStatus = MB_ENOREG;
	}
	return eStatus;
}

/**
  *****************************************************************************
  * @Name   : �������ּĴ���
  *
  * @Brief  : ��Ӧ������0x06 -> eMBFuncWriteHoldingRegister
  *                    0x16 -> eMBFuncWriteMultipleHoldingRegister
  *                    0x03 -> eMBFuncReadHoldingRegister
  *                    0x23 -> eMBFuncReadWriteMultipleHoldingRegister
  *
  * @Input  : *pucRegBuffer�����ݻ���������Ӧ������
  *           usAddress:     �Ĵ�����ַ
  *           usNRegs:       �����Ĵ�������
  *           eMode:         ������
  *
  * @Output : none
  *
  * @Return : Modbus״̬��Ϣ
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
	//�жϵ�ַ�Ƿ�Ϸ�
	if((usAddress >= REG_HOLDING_START) && ((usAddress + usNRegs) <= (REG_HOLDING_START + REG_HOLDING_NREGS)))
	{
		iRegIndex = (int)(usAddress - usRegHoldStart);	//�õ�ƫ����
		//
		//���ݹ�������в���
		//
		switch(eMode)
		{
			case MB_REG_READ:  //�����ּĴ���
					while(usNRegs > 0)
					{
						*pucRegBuffer++ = (uint8_t)(usRegHoldBuf[iRegIndex] >> 8);  //��8λ�ֽ�
						*pucRegBuffer++ = (uint8_t)(usRegHoldBuf[iRegIndex] & 0xFF); //��8λ�ֽ�
						iRegIndex++;
						usNRegs--;
					}                            
					break;
					
			case MB_REG_WRITE:  //д���ּĴ���
					while(usNRegs > 0)
					{
						usRegHoldBuf[iRegIndex] = *pucRegBuffer++ << 8;  //��8λ�ֽ�
						usRegHoldBuf[iRegIndex] |= *pucRegBuffer++;  //��8λ�ֽ�
						iRegIndex++;
						usNRegs--;
					}
					//���²���
					if(((usAddress - usRegHoldStart) == 0) && (iNRegs == 84))
					{
						//�յ��˸��²��������ݣ������������׵�ַ��ָ�뷢���豸�����߳�
						appMsg.dataType = EV_SET_PARA;
						appMsg.pVoid = usRegHoldBuf;
						me->sendEvent(&appMsg);
					}
					else if(((usAddress - usRegHoldStart) == 98) && (iNRegs == 2))
					{
						//��ʱ�յ�������Ϊ���ӱ��������ȣ�������DEBUGģʽ����Ч
						appMsg.dataType = EV_SET_PROPOR;
						appMsg.pVoid = &usRegHoldBuf[98];
						me->sendEvent(&appMsg);
					}
					break;
		}
	}
	else  //�����ַ
	{
		eStatus = MB_ENOREG;
	}
	return eStatus;
}

/**
  *****************************************************************************
  * @Name   : ������Ȧ
  *
  * @Brief  : ��Ӧ������0x01 -> eMBFuncReadCoils
  *                    0x05 -> eMBFuncWriteCoil
  *                    0x15 -> д�����Ȧ eMBFuncWriteMultipleCoils
  *
  * @Input  : *pucRegBuffer�����ݻ���������Ӧ������
  *           usAddress:     �Ĵ�����ַ
  *           usNCoils:      ������Ȧ����
  *           eMode:         ������
  *
  * @Output : none
  *
  * @Return : Modbus״̬��Ϣ
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
	//�жϵ�ַ�Ϸ���
	if ((usAddress >= COIL_START) && ((usAddress + usNCoils) <= (COIL_START + COIL_NCOILS)))
	{
		usBitOffset = usAddress - COIL_START;
		//
		//���ݹ��������
		//
		switch (eMode)
		{
			case MB_REG_READ:  //��ȡ����
				while (iNCoils > 0)
				{
					*pucRegBuffer++ = xMBUtilGetBits(ucCoilBuf, usBitOffset, (u8)(iNCoils > 8 ? 8: iNCoils));
					iNCoils -= 8;
					usBitOffset += 8;
				}
				break;
			case MB_REG_WRITE:  //д����
				while (iNCoils > 0)
				{
					xMBUtilSetBits(ucCoilBuf, usBitOffset, (u8)(iNCoils > 8 ? 8: iNCoils), *pucRegBuffer++);
					iNCoils -= 8;	
				}
				/*--------------------------------������Ȧ��ͬ״ִ̬�в�ͬ����----------------------------------*/
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
					//�������Ʒ��ŵ�ʱ�򣬸��ݷ��ű�������豸�����̷߳��ͷ��ſ�����Ϣ
					if(ucCoilBuf[usBitOffset / 8] & (1 << (usBitOffset % 8)))
						appMsg.pVoid = (void *)(0x80000000 | (1 << usBitOffset));	//ֱ�ӷ���ָ��
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
	else  //�����ַ
	{
		eStatus = MB_ENOREG;
	}
	
	return eStatus;
}

/**
  *****************************************************************************
  * @Name   : ������ɢ�Ĵ���
  *
  * @Brief  : ��Ӧ������0x02 -> eMBFuncReadDiscreteInputs
  *
  * @Input  : *pucRegBuffer�����ݻ���������Ӧ������
  *           usAddress:     �Ĵ�����ַ
  *           usNDiscrete:   �����Ĵ�������
  *
  * @Output : none
  *
  * @Return : Modbus״̬��Ϣ
  *****************************************************************************
**/
eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
	eMBErrorCode eStatus = MB_ENOERR;
	short iNDiscrete = (short)usNDiscrete;
	u16 usBitOffset = 0;
	
	//
	//�жϵ�ַ�Ϸ���
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
	else  //�����ַ
	{
		eStatus = MB_ENOREG;
	}
	
	return eStatus;
}

