#include "mbTcp_protocol.h"
#include "F28_Application.h"
#include <string.h>



/**
  * 函数功能：将数据进行打包处理
  * 输入参数：u8SendBuff 拼包成功后需要发送的数据
  *          protFrame_send 入口参数
  * 返 回 值：>0 数据长度， -1 拼包失败
  */
int32_t SendDataStructPack(uint8_t *u8SendBuff, MBTCP_PROTOCOL_FRAME protFrame_send)
{
	int i = 0;
	int32_t buffSize = -1;
	if(u8SendBuff == NULL)
	{
		return -1;
	}
	u8SendBuff[0] = protFrame_send.u16TID >> 8;
	u8SendBuff[1] = protFrame_send.u16TID & 0xFF;
	u8SendBuff[2] = protFrame_send.u16PID >> 8;
	u8SendBuff[3] = protFrame_send.u16PID & 0xFF;
	u8SendBuff[4] = protFrame_send.u16ByteLen >> 8;
	u8SendBuff[5] = protFrame_send.u16ByteLen & 0xFF;
	u8SendBuff[6] = protFrame_send.u8UID;
	switch(protFrame_send.u8FunCode)
	{
		case TYPE_READ_HOLDING:
			u8SendBuff[7] = protFrame_send.u8FunCode;
			u8SendBuff[8] = protFrame_send.u16RegStartingAddr >> 8;
			u8SendBuff[9] = protFrame_send.u16RegStartingAddr & 0xFF;
			u8SendBuff[10] = protFrame_send.u16RegCount >> 8;
			u8SendBuff[11] = protFrame_send.u16RegCount & 0xFF;
			buffSize = protFrame_send.u16ByteLen + 6;
			break;
		case TYPE_WRITE_HOLDING:
			u8SendBuff[7] = protFrame_send.u8FunCode;
			u8SendBuff[8] = protFrame_send.u16RegStartingAddr >> 8;
			u8SendBuff[9] = protFrame_send.u16RegStartingAddr & 0xFF;
			u8SendBuff[10] = protFrame_send.u16RegCount >> 8;
			u8SendBuff[11] = protFrame_send.u16RegCount & 0xFF;
			u8SendBuff[12] = protFrame_send.u8ByteCount;
			for(; i < protFrame_send.u8ByteCount; i++)
			{
				if(i % 2 == 0)
					u8SendBuff[13+i] = protFrame_send.u16dataSeg[i/2] >> 8;
				else
					u8SendBuff[13+i] = protFrame_send.u16dataSeg[i/2] & 0xFF;
					
			}
			buffSize = protFrame_send.u16ByteLen + 6;
			break;
		default:
			buffSize = -1;
			break;		
	}
	return buffSize;
}

//解析收到的一帧数据
int32_t ReceDataStructUnpack(uint8_t *u8ReceBuff, MBTCP_PROTOCOL_FRAME protFrame_rece)
{
	int i = 0;
	int32_t frameLen = -1;
	protFrame_rece.u16TID = (u8ReceBuff[0]<<8) | u8ReceBuff[1];
	protFrame_rece.u16PID = (u8ReceBuff[2]<<8) | u8ReceBuff[3];
	protFrame_rece.u16ByteLen = (u8ReceBuff[4]<<8) | u8ReceBuff[5];
	protFrame_rece.u8UID = u8ReceBuff[6];
	protFrame_rece.u8FunCode = u8ReceBuff[7];
	switch(protFrame_rece.u8FunCode)
	{
		case TYPE_READ_HOLDING:
			protFrame_rece.u8ByteCount = u8ReceBuff[8];
			memcpy(protFrame_rece.u8DataSeg, &u8ReceBuff[9], protFrame_rece.u8ByteCount);
			frameLen = protFrame_rece.u16ByteLen + 6;
			if(protFrame_rece.u16TID == READ_REALTIME_TID)
			{
				F28_GetRealTimeData(&protFrame_rece, )
			}
			break;
		case TYPE_WRITE_HOLDING:
			protFrame_rece.u16RegStartingAddr = (u8ReceBuff[8] << 8) | u8ReceBuff[9];
			protFrame_rece.u16RegCount = (u8ReceBuff[10] << 8) | u8ReceBuff[11];
			frameLen = protFrame_rece.u16ByteLen + 6;
			break;
		default:
			frameLen = -1;
			break;
	}
	return frameLen;	
}

