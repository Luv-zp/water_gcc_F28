#include "F28_Application.h"
#include "universalType.h"
#include "userApp.h"

//static F28_PARAMETERS paraSetting;
static uint8_t u8SendBuff[200];

SHORT F28_SetModuleParameters(MBTCP_PROTOCOL_FRAME* pFrame, F28_PARAMETERS* pPara)
{
	int32_t dataSize = 0;
	AppMessage msg;
	pFrame->u16TID = SET_PARA_TID;
	pFrame->u16PID = SET_PARA_PID;
	pFrame->u16ByteLen = 0x0055;
	pFrame->u8UID = 0x01;
	pFrame->u8FunCode = 0x10;
	pFrame->u16RegStartingAddr = 0x2000;
	pFrame->u16RegCount = 0x0027;
	pFrame->u8ByteCount = 0x4E;
	
	pFrame->u16dataSeg[0] = 0x6600;
	pFrame->u16dataSeg[1] = pPara->wTypeTest;
	pFrame->u16dataSeg[2] = pPara->wTpsFillVol;
	pFrame->u16dataSeg[3] = pPara->wTpsTransfert;
	pFrame->u16dataSeg[4] = pPara->wTpsFill;
	pFrame->u16dataSeg[5] = pPara->wTpsStab;
	pFrame->u16dataSeg[6] = pPara->wTpsTest;
	pFrame->u16dataSeg[7] = pPara->wTpsDump;
	pFrame->u16dataSeg[8] = pPara->wTpsDump;
	pFrame->u16dataSeg[9] = pPara->wPress1Unit;
	pFrame->u16dataSeg[10] = 0x0000;
	pFrame->u16dataSeg[11] = 0x0000;
	pFrame->u16dataSeg[12] = 0x0000;
	pFrame->u16dataSeg[13] = 0x0000;
	pFrame->u16dataSeg[14] = 0x0000;
	pFrame->u16dataSeg[15] = 0x0000;
	pFrame->u16dataSeg[16] = 0x0000;
	pFrame->u16dataSeg[17] = 0x0000;
	pFrame->u16dataSeg[18] = 0x0000;
	pFrame->u16dataSeg[19] = 0x0000;
	pFrame->u16dataSeg[20] = pPara->wFillMode;
	pFrame->u16dataSeg[21] = pPara->wLeakUnit;
	pFrame->u16dataSeg[22] = pPara->wRejectCalc;
	pFrame->u16dataSeg[23] = pPara->wVolumeUnit;
	pFrame->u16dataSeg[24] = 0x0000;
	pFrame->u16dataSeg[25] = 0x0000;
	pFrame->u16dataSeg[26] = 0x0000;
	pFrame->u16dataSeg[27] = 0x0000;
	pFrame->u16dataSeg[28] = 0x0000;
	pFrame->u16dataSeg[29] = 0x0000;
	pFrame->u16dataSeg[30] = 0x0000;
	pFrame->u16dataSeg[31] = 0x0000;
	pFrame->u16dataSeg[32] = pPara->wOptions;
	pFrame->u16dataSeg[33] = 0x0000;
	pFrame->u16dataSeg[34] = 0x0000;
	pFrame->u16dataSeg[35] = 0x0000;
	pFrame->u16dataSeg[36] = 0x0000;
	pFrame->u16dataSeg[37] = pPara->fFilterTime;
	pFrame->u16dataSeg[38] = 0x0000;
	pFrame->u16dataSeg[39] = 0x0000;

	dataSize = SendDataStructPack(u8SendBuff, *pFrame);
	if(dataSize == -1)
		return -1;
	msg.dataType = dataSize;
	msg.pVoid = u8SendBuff;
	xQueueSend(client_netQ, &msg, 0);		//不死等队列有空闲位置
	return dataSize;
}

SHORT F28_ReadModuleParameters(MBTCP_PROTOCOL_FRAME* pFrame, F28_PARAMETERS* pPara)
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
	
	dataSize = SendDataStructPack(u8SendBuff, *pFrame);
	if(dataSize == -1)
		return -1;
	msg.dataType = dataSize;
	msg.pVoid = u8SendBuff;
	xQueueSend(client_netQ, &msg, 0);		//不死等队列有空闲位置
	return dataSize;
}

SHORT F28_SetFillTime(MBTCP_PROTOCOL_FRAME* pFrame, WORD* pTime)
{
	int32_t dataSize = 0;
	AppMessage msg;
	pFrame->u16TID = SET_PARA_TID;
	pFrame->u16PID = SET_PARA_PID;
	pFrame->u16ByteLen = 0x000B;
	pFrame->u8UID = 0x01;
	pFrame->u8FunCode = 0x10;
	pFrame->u16RegStartingAddr = 0x2004;
	pFrame->u16RegCount = 0x0001;
	pFrame->u8ByteCount = 0x04;
	
	pFrame->u16dataSeg[0] = *pTime;
	
	dataSize = SendDataStructPack(u8SendBuff, *pFrame);
	if(dataSize == -1)
		return -1;
	msg.dataType = dataSize;
	msg.pVoid = u8SendBuff;
	xQueueSend(client_netQ, &msg, 0);		//不死等队列有空闲位置
	return dataSize;
}

SHORT F28_SetStableTime(MBTCP_PROTOCOL_FRAME* pFrame, WORD* pTime)
{
	int32_t dataSize = 0;
	AppMessage msg;
	pFrame->u16TID = SET_PARA_TID;
	pFrame->u16PID = SET_PARA_PID;
	pFrame->u16ByteLen = 0x000B;
	pFrame->u8UID = 0x01;
	pFrame->u8FunCode = 0x10;
	pFrame->u16RegStartingAddr = 0x2005;
	pFrame->u16RegCount = 0x0001;
	pFrame->u8ByteCount = 0x04;
	
	pFrame->u16dataSeg[0] = *pTime;
	
	dataSize = SendDataStructPack(u8SendBuff, *pFrame);
	if(dataSize == -1)
		return -1;
	msg.dataType = dataSize;
	msg.pVoid = u8SendBuff;
	xQueueSend(client_netQ, &msg, 0);		//不死等队列有空闲位置
	return dataSize;
}

SHORT F28_SetTestTime(MBTCP_PROTOCOL_FRAME* pFrame, WORD* pTime)
{
	int32_t dataSize = 0;
	AppMessage msg;
	pFrame->u16TID = SET_PARA_TID;
	pFrame->u16PID = SET_PARA_PID;
	pFrame->u16ByteLen = 0x000B;
	pFrame->u8UID = 0x01;
	pFrame->u8FunCode = 0x10;
	pFrame->u16RegStartingAddr = 0x2006;
	pFrame->u16RegCount = 0x0001;
	pFrame->u8ByteCount = 0x04;
	
	pFrame->u16dataSeg[0] = *pTime;
	
	dataSize = SendDataStructPack(u8SendBuff, *pFrame);
	if(dataSize == -1)
		return -1;
	msg.dataType = dataSize;
	msg.pVoid = u8SendBuff;
	xQueueSend(client_netQ, &msg, 0);		//不死等队列有空闲位置
	return dataSize;
}

SHORT F28_SetDumpTime(MBTCP_PROTOCOL_FRAME* pFrame, WORD* pTime)
{
	int32_t dataSize = 0;
	AppMessage msg;
	pFrame->u16TID = SET_PARA_TID;
	pFrame->u16PID = SET_PARA_PID;
	pFrame->u16ByteLen = 0x000B;
	pFrame->u8UID = 0x01;
	pFrame->u8FunCode = 0x10;
	pFrame->u16RegStartingAddr = 0x2007;
	pFrame->u16RegCount = 0x0001;
	pFrame->u8ByteCount = 0x04;
	
	pFrame->u16dataSeg[0] = *pTime;
	
	dataSize = SendDataStructPack(u8SendBuff, *pFrame);
	if(dataSize == -1)
		return -1;
	msg.dataType = dataSize;
	msg.pVoid = u8SendBuff;
	xQueueSend(client_netQ, &msg, 0);		//不死等队列有空闲位置
	return dataSize;
}

SHORT F28_StartCycle(MBTCP_PROTOCOL_FRAME* pFrame)
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
	
	pFrame->u16dataSeg[0] = 0x1200;
	pFrame->u16dataSeg[1] = 0x0000;
	
	dataSize = SendDataStructPack(u8SendBuff, *pFrame);
	if(dataSize == -1)
		return -1;
	msg.dataType = dataSize;
	msg.pVoid = u8SendBuff;
	xQueueSend(client_netQ, &msg, 0);		//不死等队列有空闲位置
	return dataSize;
}

SHORT F28_StopCycle(MBTCP_PROTOCOL_FRAME* pFrame)
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
	
	pFrame->u16dataSeg[0] = 0x1300;
	pFrame->u16dataSeg[1] = 0x0000;
	
	dataSize = SendDataStructPack(u8SendBuff, *pFrame);
	if(dataSize == -1)
		return -1;
	msg.dataType = dataSize;
	msg.pVoid = u8SendBuff;
	xQueueSend(client_netQ, &msg, 0);		//不死等队列有空闲位置
	return dataSize;
}

SHORT F28_GetRealTimeData(MBTCP_PROTOCOL_FRAME* pFrame, F28_REALTIME_CYCLE* pCycle)
{
	if((pFrame == 0) || (pCycle == 0))
		return 0;
	pCycle->wEndCycle = (pFrame->u8DataSeg[3] << 8) | (pFrame->u8DataSeg[2]);
	pCycle->wStatus = (pFrame->u8dataSeg[5] << 8) | (pFrame->u8dataSeg[4]);
	pCycle->fPressureValue = (pFrame->u8dataSeg[9] << 24) | (pFrame->u8dataSeg[8] << 16) | (pFrame->u8dataSeg[7] << 8) | (pFrame->u8dataSeg[6]);
	pCycle->wUnitPressure = (pFrame->u8dataSeg[11] << 8) | (pFrame->u8dataSeg[10]);
	pCycle->fLeakValue = (pFrame->u8dataSeg[15] << 24) | (pFrame->u8dataSeg[14] << 16) | (pFrame->u8dataSeg[13] << 8) | (pFrame->u8dataSeg[12]);
	pCycle->wUnitLeak = (pFrame->u8dataSeg[17] << 8) | (pFrame->u8dataSeg[16]);
	pCycle->fPatm = (pFrame->u8dataSeg[21] << 24) | (pFrame->u8dataSeg[20] << 16) | (pFrame->u8dataSeg[19] << 8) | (pFrame->u8dataSeg[18]);
	pCycle->fInternalTemperature = (pFrame->u8dataSeg[25] << 24) | (pFrame->u8dataSeg[24] << 16) | (pFrame->u8dataSeg[23] << 8) | (pFrame->u8dataSeg[22]);
	return 1;
}

SHORT F28_Send_ReadRealTimeDataFrame(MBTCP_PROTOCOL_FRAME* pFrame)
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
	
	dataSize = SendDataStructPack(u8SendBuff, *pFrame);
	if(dataSize == -1)
		return -1;
	msg.dataType = dataSize;
	msg.pVoid = u8SendBuff;
	xQueueSend(client_netQ, &msg, 0);		//不死等队列有空闲位置
	return dataSize;
}

SHORT F28_GetLastResult(MBTCP_PROTOCOL_FRAME* pFrame, F28_LAST_RESULT* pResult)
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
	
	dataSize = SendDataStructPack(u8SendBuff, *pFrame);
	if(dataSize == -1)
		return -1;
	msg.dataType = dataSize;
	msg.pVoid = u8SendBuff;
	xQueueSend(client_netQ, &msg, 0);		//不死等队列有空闲位置
	return dataSize;
}
