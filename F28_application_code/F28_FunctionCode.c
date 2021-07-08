#include "F28_FunctionCode.h"
#include "universalType.h"
#include "userApp.h"
#include <string.h>
#include "malloc.h"
#include "GeneralFunc.h"
#include "BLL_Modbus_Server.h"
#include "BLL_Alarm.h"

static SHORT F28_Init(CF28Light * me);
static SHORT F28_Open(CF28Light * me);	
static void F28_Close(CF28Light * me);
static SHORT F28_IsConnected(CF28Light * me);

static SHORT F28_StartCycle(CF28Light * me, MBTCP_PROTOCOL_FRAME* pFrame);
static SHORT F28_StopCycle(CF28Light * me, MBTCP_PROTOCOL_FRAME* pFrame);	

static SHORT F28_SetModuleParameters(CF28Light * me, MBTCP_PROTOCOL_FRAME* pFrame);
//static SHORT F28_Send_ReadModuleParametersFrame(CF28Light * me, MBTCP_PROTOCOL_FRAME* pFrame);
//static SHORT F28_GetModuleParameters(CF28Light * me, MBTCP_PROTOCOL_FRAME* pFrame, F28_PARAMETERS* pPara);

static SHORT F28_Send_ReadRealTimeDataFrame(CF28Light * me, MBTCP_PROTOCOL_FRAME* pFrame);
static SHORT F28_GetRealTimeData(CF28Light * me, MBTCP_PROTOCOL_FRAME* pFrame);

static SHORT F28_Send_ReadLastResultFrame(CF28Light * me, MBTCP_PROTOCOL_FRAME* pFrame);
static SHORT F28_GetLastResult(CF28Light * me, MBTCP_PROTOCOL_FRAME* pFrame);


static CF28Light CF28LightDev;
static F28_PARAMETERS		F28_Parametes;
static F28_REALTIME_CYCLE	F28_RealTimeData;
static F28_LAST_RESULT		F28_LastResult;



CF28Light * CF28Light_create(void)
{
	static CF28Light * me = 0;
	if(me == 0)
	{
		me = & CF28LightDev;
		me->Init = F28_Init;
		me->Open = F28_Open;
		me->Close = F28_Close;
		me->IsConnected = F28_IsConnected;
		
		me->StartCycle = F28_StartCycle;
		me->StopCycle = F28_StopCycle;
		
		me->SetModuleParameters = F28_SetModuleParameters;
//		me->Send_ReadModuleParametersFrame = F28_Send_ReadModuleParametersFrame;
//		me->GetModuleParameters = F28_GetModuleParameters;
		
		me->Send_ReadRealTimeDataFrame = F28_Send_ReadRealTimeDataFrame;
		me->GetRealTimeData = F28_GetRealTimeData;
		
		me->Send_ReadLastResultFrame = F28_Send_ReadLastResultFrame;
		me->GetLastResult = F28_GetLastResult;
		
		me->parametes = &F28_Parametes;
		me->realTimeData = &F28_RealTimeData;
		me->lastResult = &F28_LastResult;
		
		me->Init(me);
	}
	return me;
}

static SHORT F28_Init(CF28Light * me)
{
	return 0;
}

static SHORT F28_Open(CF28Light * me)
{
	return 0;
}

static void F28_Close(CF28Light * me)
{
}

static SHORT F28_IsConnected(CF28Light * me)
{
	return 0;
}


static SHORT F28_StartCycle(CF28Light * me, MBTCP_PROTOCOL_FRAME* pFrame)
{
	int32_t dataSize = 0;
	AppMessage msg;
	pFrame->u16TID = START_CIRCLE_TID;
	pFrame->u16PID = START_CIRCLE_PID;
	pFrame->u16ByteLen = 0x000B;
	pFrame->u8UID = 0x01;
	pFrame->u8FunCode = 0x10;
	pFrame->u16RegStartingAddr = 0x2500;
	pFrame->u16RegCount = 0x0002;
	pFrame->u8ByteCount = 0x04;
	pFrame->u8DataSeg[0] = 0x12;
	pFrame->u8DataSeg[1] = 0x00;
	pFrame->u8DataSeg[2] = 0x00;
	pFrame->u8DataSeg[3] = 0x00;
	
	memset(F28_SendBuf, 0, F28_TxBufSize);
	dataSize = SendDataStructPack(F28_SendBuf, *pFrame);
	if(dataSize == -1)
		return -1;
	msg.dataType = dataSize;
	msg.pVoid = F28_SendBuf;
	xQueueSend(client_netQ, &msg, 0);		//不死等队列有空闲位置
	return dataSize;
}

static SHORT F28_StopCycle(CF28Light * me, MBTCP_PROTOCOL_FRAME* pFrame)
{
	int32_t dataSize = 0;
	AppMessage msg;
	pFrame->u16TID = STOP_CIRCLE_TID;
	pFrame->u16PID = STOP_CIRCLE_PID;
	pFrame->u16ByteLen = 0x000B;
	pFrame->u8UID = 0x01;
	pFrame->u8FunCode = 0x10;
	pFrame->u16RegStartingAddr = 0x2500;
	pFrame->u16RegCount = 0x0002;
	pFrame->u8ByteCount = 0x04;	
	
//	pFrame->u8DataSeg = mymalloc(SRAMIN, pFrame->u8ByteCount); //动态申请内存（针对指针）
	pFrame->u8DataSeg[0] = 0x13;
	pFrame->u8DataSeg[1] = 0x00;
	pFrame->u8DataSeg[2] = 0x00;
	pFrame->u8DataSeg[3] = 0x00;
	
	memset(F28_SendBuf, 0, F28_TxBufSize);
	dataSize = SendDataStructPack(F28_SendBuf, *pFrame);
	if(dataSize == -1)
	{
		myfree(SRAMIN, pFrame->u8DataSeg);	//释放申请的内存
		return -1;
	}
	msg.dataType = dataSize;
	msg.pVoid = F28_SendBuf;
	xQueueSend(client_netQ, &msg, 0);		//不死等队列有空闲位置
//	myfree(SRAMIN, pFrame->u8DataSeg);	//释放申请的内存
	return dataSize;
}	

static SHORT F28_SetModuleParameters(CF28Light * me, MBTCP_PROTOCOL_FRAME* pFrame)
{
	int32_t dataSize = 0;
	AppMessage msg;
	if(me->parametes == 0)
		return -1;
	pFrame->u16TID = SET_PARA_TID;
	pFrame->u16PID = SET_PARA_PID;
	pFrame->u16ByteLen = 0x0055;
	pFrame->u8UID = 0x01;
	pFrame->u8FunCode = 0x10;
	pFrame->u16RegStartingAddr = 0x2000;
	pFrame->u16RegCount = 0x0027;
	pFrame->u8ByteCount = 0x4E;
	
//	pFrame->u8DataSeg = mymalloc(SRAMIN, pFrame->u8ByteCount); //动态申请内存
	pFrame->u8DataSeg[0] = 0x66;
	pFrame->u8DataSeg[1] = 0x00;
	pFrame->u8DataSeg[2] = me->parametes->wTypeTest;
	pFrame->u8DataSeg[3] = me->parametes->wTypeTest  >>8;
	pFrame->u8DataSeg[4] = me->parametes->wTpsFillVol;
	pFrame->u8DataSeg[5] = me->parametes->wTpsFillVol >> 8;
	pFrame->u8DataSeg[6] = me->parametes->wTpsTransfert;
	pFrame->u8DataSeg[7] = me->parametes->wTpsTransfert >> 8;
	pFrame->u8DataSeg[8] = me->parametes->wTpsFill;
	pFrame->u8DataSeg[9] = me->parametes->wTpsFill >> 8; 
	pFrame->u8DataSeg[10] = me->parametes->wTpsStab;
	pFrame->u8DataSeg[11] = me->parametes->wTpsStab >> 8;
	pFrame->u8DataSeg[12] = me->parametes->wTpsTest;
	pFrame->u8DataSeg[13] = me->parametes->wTpsTest >> 8;
	pFrame->u8DataSeg[14] = me->parametes->wTpsDump;
	pFrame->u8DataSeg[15] = me->parametes->wTpsDump >> 8;
	pFrame->u8DataSeg[16] = me->parametes->wPress1Unit;
	pFrame->u8DataSeg[17] = me->parametes->wPress1Unit >> 8;
	
	pFrame->u8DataSeg[18] = F28_floatDataHandle_X100(me->parametes->fPress1Min);
	pFrame->u8DataSeg[19] = F28_floatDataHandle_X100(me->parametes->fPress1Min) >> 8;
	pFrame->u8DataSeg[20] = F28_floatDataHandle_X100(me->parametes->fPress1Min) >> 16;
	pFrame->u8DataSeg[21] = F28_floatDataHandle_X100(me->parametes->fPress1Min) >> 24;
	
	pFrame->u8DataSeg[22] = F28_floatDataHandle_X100(me->parametes->fPress1Max);
	pFrame->u8DataSeg[23] = F28_floatDataHandle_X100(me->parametes->fPress1Max) >> 8;
	pFrame->u8DataSeg[24] = F28_floatDataHandle_X100(me->parametes->fPress1Max) >> 16;
	pFrame->u8DataSeg[25] = F28_floatDataHandle_X100(me->parametes->fPress1Max) >> 24;
	
	pFrame->u8DataSeg[26] = F28_floatDataHandle_X100(me->parametes->fSetFillP1);
	pFrame->u8DataSeg[27] = F28_floatDataHandle_X100(me->parametes->fSetFillP1) >> 8;
	pFrame->u8DataSeg[28] = F28_floatDataHandle_X100(me->parametes->fSetFillP1) >> 16;
	pFrame->u8DataSeg[29] = F28_floatDataHandle_X100(me->parametes->fSetFillP1) >> 24;
	
	pFrame->u8DataSeg[30] = F28_floatDataHandle_X100(me->parametes->fRatioMax);
	pFrame->u8DataSeg[31] = F28_floatDataHandle_X100(me->parametes->fRatioMax) >> 8;
	pFrame->u8DataSeg[32] = F28_floatDataHandle_X100(me->parametes->fRatioMax) >> 16;
	pFrame->u8DataSeg[33] = F28_floatDataHandle_X100(me->parametes->fRatioMax) >> 24;
	
	pFrame->u8DataSeg[34] = F28_floatDataHandle_X100(me->parametes->fRatioMin);
	pFrame->u8DataSeg[35] = F28_floatDataHandle_X100(me->parametes->fRatioMin) >> 8;
	pFrame->u8DataSeg[36] = F28_floatDataHandle_X100(me->parametes->fRatioMin) >> 16;
	pFrame->u8DataSeg[37] = F28_floatDataHandle_X100(me->parametes->fRatioMin) >> 24;
	
	pFrame->u8DataSeg[38] = me->parametes->wFillMode;
	pFrame->u8DataSeg[39] = me->parametes->wFillMode >> 8;
	pFrame->u8DataSeg[40] = me->parametes->wLeakUnit;
	pFrame->u8DataSeg[41] = me->parametes->wLeakUnit >> 8;
	pFrame->u8DataSeg[42] = me->parametes->wRejectCalc;
	pFrame->u8DataSeg[43] = me->parametes->wRejectCalc >> 8;
	pFrame->u8DataSeg[44] = me->parametes->wVolumeUnit;
	pFrame->u8DataSeg[45] = me->parametes->wVolumeUnit >> 8;
	
	pFrame->u8DataSeg[46] = convFloat2u32(me->parametes->fVolume);
	pFrame->u8DataSeg[47] = convFloat2u32(me->parametes->fVolume) >> 8;
	pFrame->u8DataSeg[48] = convFloat2u32(me->parametes->fVolume) >> 16;
	pFrame->u8DataSeg[49] = convFloat2u32(me->parametes->fVolume) >> 24;
	
	pFrame->u8DataSeg[50] = F28_floatDataHandle_X1000(me->parametes->fRejectMin);
	pFrame->u8DataSeg[51] = F28_floatDataHandle_X1000(me->parametes->fRejectMin) >> 8;
	pFrame->u8DataSeg[52] = F28_floatDataHandle_X1000(me->parametes->fRejectMin) >> 16;
	pFrame->u8DataSeg[53] = F28_floatDataHandle_X1000(me->parametes->fRejectMin) >> 24;
	
	pFrame->u8DataSeg[54] = F28_floatDataHandle_X1000(me->parametes->fRejectMax);
	pFrame->u8DataSeg[55] = F28_floatDataHandle_X1000(me->parametes->fRejectMax) >> 8;
	pFrame->u8DataSeg[56] = F28_floatDataHandle_X1000(me->parametes->fRejectMax) >> 16;
	pFrame->u8DataSeg[57] = F28_floatDataHandle_X1000(me->parametes->fRejectMax) >> 24;
	
	pFrame->u8DataSeg[58] = F28_floatDataHandle_X100(me->parametes->fCoeffAutoFill);
	pFrame->u8DataSeg[59] = F28_floatDataHandle_X100(me->parametes->fCoeffAutoFill) >> 8;
	pFrame->u8DataSeg[60] = F28_floatDataHandle_X100(me->parametes->fCoeffAutoFill) >> 16;
	pFrame->u8DataSeg[61] = F28_floatDataHandle_X100(me->parametes->fCoeffAutoFill) >> 24;
	
	pFrame->u8DataSeg[62] = me->parametes->wOptions;
	pFrame->u8DataSeg[63] = me->parametes->wOptions >> 8;
	
	pFrame->u8DataSeg[64] = convFloat2u32(me->parametes->fPatmSTD);
	pFrame->u8DataSeg[65] = convFloat2u32(me->parametes->fPatmSTD) >> 8;
	pFrame->u8DataSeg[66] = convFloat2u32(me->parametes->fPatmSTD) >> 16;
	pFrame->u8DataSeg[67] = convFloat2u32(me->parametes->fPatmSTD) >> 24;
	
	pFrame->u8DataSeg[68] = convFloat2u32(me->parametes->fTempSTD);
	pFrame->u8DataSeg[69] = convFloat2u32(me->parametes->fTempSTD) >> 8;
	pFrame->u8DataSeg[70] = convFloat2u32(me->parametes->fTempSTD) >> 16;
	pFrame->u8DataSeg[71] = convFloat2u32(me->parametes->fTempSTD) >> 24;
	
	pFrame->u8DataSeg[72] = me->parametes->fFilterTime;
	pFrame->u8DataSeg[73] = me->parametes->fFilterTime >> 8;
	
	pFrame->u8DataSeg[74] = F28_floatDataHandle_X10(me->parametes->fOffsetLeak);
	pFrame->u8DataSeg[75] = F28_floatDataHandle_X10(me->parametes->fOffsetLeak) >> 8;
	pFrame->u8DataSeg[76] = F28_floatDataHandle_X10(me->parametes->fOffsetLeak) >> 16;
	pFrame->u8DataSeg[77] = F28_floatDataHandle_X10(me->parametes->fOffsetLeak) >> 24;
	
	memset(F28_SendBuf, 0, F28_TxBufSize);
	dataSize = SendDataStructPack(F28_SendBuf, *pFrame);
	if(dataSize == -1)
	{
		myfree(SRAMIN, pFrame->u8DataSeg);	//释放申请的内存
		return -1;
	}
	msg.dataType = dataSize;
	msg.pVoid = F28_SendBuf;
	xQueueSend(client_netQ, &msg, 0);		//不死等队列有空闲位置
//	myfree(SRAMIN, pFrame->u8DataSeg);	//释放申请的内存
	return dataSize;
}

//暂时不用
/*
static SHORT F28_Send_ReadModuleParametersFrame(CF28Light * me, MBTCP_PROTOCOL_FRAME* pFrame)
{
	int32_t dataSize = 0;
	AppMessage msg;
	pFrame->u16TID = READ_PARA_TID;
	pFrame->u16PID = READ_PARA_PID;
	pFrame->u16ByteLen = 0x0006;
	pFrame->u8UID = 0x01;
	pFrame->u8FunCode = 0x03;
	pFrame->u16RegStartingAddr = 0x2000;
	pFrame->u16RegCount = 0x0027;
	pFrame->u8DataSeg = 0;
	memset(F28_SendBuf, 0, F28_TxBufSize);
	dataSize = SendDataStructPack(F28_SendBuf, *pFrame);
	if(dataSize == -1)
		return -1;
	msg.dataType = dataSize;
	msg.pVoid = F28_SendBuf;
	xQueueSend(client_netQ, &msg, 0);		//不死等队列有空闲位置
	return dataSize;
}
*/

//static SHORT F28_GetModuleParameters(CF28Light * me, MBTCP_PROTOCOL_FRAME* pFrame, F28_PARAMETERS* pPara)
//{
//}

static SHORT F28_Send_ReadRealTimeDataFrame(CF28Light * me, MBTCP_PROTOCOL_FRAME* pFrame)
{
	int32_t dataSize = 0;
	AppMessage msg;
	pFrame->u16TID = READ_REALTIME_TID;
	pFrame->u16PID = READ_REALTIME_PID;
	pFrame->u16ByteLen = 0x0006;
	pFrame->u8UID = 0x01;
	pFrame->u8FunCode = 0x03;
	pFrame->u16RegStartingAddr = 0x2201;
	pFrame->u16RegCount = 0x000D;
//	pFrame->u8DataSeg = 0;
	
	memset(F28_SendBuf, 0, F28_TxBufSize);
	dataSize = SendDataStructPack(F28_SendBuf, *pFrame);
	if(dataSize == -1)
		return -1;
	msg.dataType = dataSize;
	msg.pVoid = F28_SendBuf;
	xQueueSend(client_netQ, &msg, 0);		//不死等队列有空闲位置
	return dataSize;
}

static SHORT F28_GetRealTimeData(CF28Light * me, MBTCP_PROTOCOL_FRAME* pFrame)
{
	AppMessage msg;
	if((pFrame == 0) || (me->realTimeData == 0))
		return 0;
	me->realTimeData->wEndCycle = (pFrame->u8DataSeg[3] << 8) | (pFrame->u8DataSeg[2]);
	me->realTimeData->wStatus = (pFrame->u8DataSeg[5] << 8) | (pFrame->u8DataSeg[4]);
	me->realTimeData->fPressureValue = ((pFrame->u8DataSeg[9] << 24) | (pFrame->u8DataSeg[8] << 16) | (pFrame->u8DataSeg[7] << 8) | (pFrame->u8DataSeg[6])) / 100.0;
	me->realTimeData->wUnitPressure = (pFrame->u8DataSeg[11] << 8) | (pFrame->u8DataSeg[10]);
	me->realTimeData->fLeakValue = ((pFrame->u8DataSeg[15] << 24) | (pFrame->u8DataSeg[14] << 16) | (pFrame->u8DataSeg[13] << 8) | (pFrame->u8DataSeg[12])) / 1000.0;
	me->realTimeData->wUnitLeak = (pFrame->u8DataSeg[17] << 8) | (pFrame->u8DataSeg[16]);
	me->realTimeData->fPatm = ((pFrame->u8DataSeg[21] << 24) | (pFrame->u8DataSeg[20] << 16) | (pFrame->u8DataSeg[19] << 8) | (pFrame->u8DataSeg[18])) / 100.0;
	me->realTimeData->fInternalTemperature = ((pFrame->u8DataSeg[25] << 24) | (pFrame->u8DataSeg[24] << 16) | (pFrame->u8DataSeg[23] << 8) | (pFrame->u8DataSeg[22])) / 100.0;
	
	msg.dataType = MB_UPDATE_ABS_PRES;
	msg.pVoid = &(me->realTimeData->fPressureValue);
	xQueueSend(modbusQ, &msg, 0);
//	msg.dataType = MB_UPDATE_HIGH_1;
//	msg.pVoid = &(me->realTimeData->fLeakValue);
//	xQueueSend(modbusQ, &msg, 0);
	return 1;
}

static SHORT F28_Send_ReadLastResultFrame(CF28Light * me, MBTCP_PROTOCOL_FRAME* pFrame)
{
	int32_t dataSize = 0;
	AppMessage msg;
	pFrame->u16TID = READ_LAST_TID;
	pFrame->u16PID = READ_LAST_PID;
	pFrame->u16ByteLen = 0x0006;
	pFrame->u8UID = 0x01;
	pFrame->u8FunCode = 0x03;
	pFrame->u16RegStartingAddr = 0x2301;
	pFrame->u16RegCount = 0x000C;
//	pFrame->u8DataSeg = 0;
	
	memset(F28_SendBuf, 0, F28_TxBufSize);
	dataSize = SendDataStructPack(F28_SendBuf, *pFrame);
	if(dataSize == -1)
		return -1;
	msg.dataType = dataSize;
	msg.pVoid = F28_SendBuf;
	xQueueSend(client_netQ, &msg, 0);		//不死等队列有空闲位置
	return dataSize;
}

static SHORT F28_GetLastResult(CF28Light * me, MBTCP_PROTOCOL_FRAME* pFrame)
{
	AppMessage msg;
	FlagHand * pErr = errHand_create();	//错误处理对象
	if((pFrame == 0) || (me->lastResult == 0))
		return 0;
	me->lastResult->wStatus = (pFrame->u8DataSeg[3] << 8) | (pFrame->u8DataSeg[2]);
	me->lastResult->fPressureValue = ((pFrame->u8DataSeg[7] << 24) | (pFrame->u8DataSeg[6] << 16) | (pFrame->u8DataSeg[5] << 8) | (pFrame->u8DataSeg[4])) / 100.0;
	me->lastResult->wUnitPressure = (pFrame->u8DataSeg[9] << 8) | (pFrame->u8DataSeg[8]);
	me->lastResult->fLeakValue = ((pFrame->u8DataSeg[13] << 24) | (pFrame->u8DataSeg[12] << 16) | (pFrame->u8DataSeg[11] << 8) | (pFrame->u8DataSeg[10])) / 1000.0;
	me->lastResult->wUnitLeak = (pFrame->u8DataSeg[15] << 8) | (pFrame->u8DataSeg[14]);
	me->lastResult->fPatm = ((pFrame->u8DataSeg[19] << 24) | (pFrame->u8DataSeg[18] << 16) | (pFrame->u8DataSeg[17] << 8) | (pFrame->u8DataSeg[16])) / 100.0;
	me->lastResult->fInternalTemperature = ((pFrame->u8DataSeg[23] << 24) | (pFrame->u8DataSeg[22] << 16) | (pFrame->u8DataSeg[21] << 8) | (pFrame->u8DataSeg[20])) / 100.0;
	
	msg.dataType = MB_UPDATE_HIGH_1;
	msg.pVoid = (void *)convFloat2u32(me->lastResult->fLeakValue);
	xQueueSend(modbusQ, &msg, 0);
	
	if(me->lastResult->fLeakValue > me->parametes->fRejectMax)
	{
		pErr->setVbRes(EX_NG1_S);
	}
	else
	{
		pErr->setVbRes(EX_OK1_S);
	}
	
	return 1;
}


