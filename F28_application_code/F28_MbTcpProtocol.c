#include "F28_MbTcpProtocol.h"
#include <string.h>

u8 F28_ReceBuf[F28_RxBufSize];
u8 F28_SendBuf[F28_TxBufSize];

/**
  * 函数功能：将数据进行打包处理
  * 输入参数：u8SendBuff 拼包成功后需要发送的数据
  *          protFrame_send 入口参数
  * 返 回 值：>0 数据长度， -1 拼包失败
  */
int32_t SendDataStructPack(uint8_t *u8SendBuff, MBTCP_PROTOCOL_FRAME protFrame_send)
{
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
			memcpy(&u8SendBuff[13], protFrame_send.u8DataSeg, protFrame_send.u8ByteCount);
			buffSize = protFrame_send.u16ByteLen + 6;
			break;
		default:
			buffSize = -1;
			break;		
	}
	return buffSize;
}

//解析收到的一帧数据
int32_t ReceDataStructUnpack(uint8_t *u8ReceBuff, MBTCP_PROTOCOL_FRAME * protFrame_rece)
{

	int32_t frameLen = -1;
	protFrame_rece->u16TID = (u8ReceBuff[0]<<8) | u8ReceBuff[1];
	protFrame_rece->u16PID = (u8ReceBuff[2]<<8) | u8ReceBuff[3];
	protFrame_rece->u16ByteLen = (u8ReceBuff[4]<<8) | u8ReceBuff[5];
	protFrame_rece->u8UID = u8ReceBuff[6];
	protFrame_rece->u8FunCode = u8ReceBuff[7];
	switch(protFrame_rece->u8FunCode)
	{
		case TYPE_READ_HOLDING:
			protFrame_rece->u8ByteCount = u8ReceBuff[8];
			memcpy(protFrame_rece->u8DataSeg, &u8ReceBuff[9], protFrame_rece->u8ByteCount);
//			protFrame_rece.u8DataSeg =  &u8ReceBuff[9]; //不采取拷贝的方式，直接用地址赋值（针对指针而不是数组）
			frameLen = protFrame_rece->u16ByteLen + 6;
			break;
		case TYPE_WRITE_HOLDING:
			protFrame_rece->u16RegStartingAddr = (u8ReceBuff[8] << 8) | u8ReceBuff[9];
			protFrame_rece->u16RegCount = (u8ReceBuff[10] << 8) | u8ReceBuff[11];
			frameLen = protFrame_rece->u16ByteLen + 6;
			break;
		default:
			frameLen = -1;
			break;
	}
	return frameLen;	
}

int F28_floatDataHandle_X100(float data)
{
	int temp = data * 100;
	return temp;
}

int F28_floatDataHandle_X10(float data)
{
	int temp = data * 10;
	return temp;
}

int F28_floatDataHandle_X1000(float data)
{
	int temp = data * 1000;
	return temp;
}

