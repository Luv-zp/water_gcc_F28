#ifndef _F28_MB_TCP_PROT_H
#define _F28_MB_TCP_PROT_H
#include "sys.h"

//定义mbtcp数据帧报文头
#define SET_PARA_TID		0x0001		//传输标识符
#define SET_PARA_PID		0x0000		//协议标识符

#define READ_PARA_TID		0x0011		//传输标识符
#define READ_PARA_PID		0x0000		//协议标识符

#define START_CIRCLE_TID	0x0002		//传输标识符
#define START_CIRCLE_PID	0x0000		//协议标识符

#define READ_REALTIME_TID	0x0003		//传输标识符 （事务处理标识）
#define READ_REALTIME_PID	0x0000		//协议标识符 （0000标识modbus tcp协议）

#define READ_LAST_TID		0x0004		//传输标识符
#define READ_LAST_PID		0x0000		//协议标识符

#define STOP_CIRCLE_TID		0x0005		//传输标识符
#define STOP_CIRCLE_PID		0x0000		//协议标识符

//功能码
#define TYPE_READ_HOLDING	0x03	//读保持寄存器
#define TYPE_WRITE_HOLDING	0x10	//写保持寄存器	


//发送、接收数据缓冲区长度
#define F28_RxBufSize 200
#define F28_TxBufSize 200

typedef struct 
{
	u16 u16TID;					//传输标识符
	u16 u16PID;					//协议标识符
	u16 u16ByteLen;				//字节长度
	u8 u8UID;					//服务器标识
	u8 u8FunCode;				//功能码
	u16 u16RegStartingAddr;		//寄存器起始地址
	u16 u16RegCount;			//寄存器个数 
	u8 u8ByteCount;				//数据段字节数
	u8 u8DataSeg[100];				//数据段	 数组而非指针
}MBTCP_PROTOCOL_FRAME;

extern u8 F28_ReceBuf[F28_RxBufSize];
extern u8 F28_SendBuf[F28_TxBufSize];
int32_t SendDataStructPack(uint8_t *u8SendBuff, MBTCP_PROTOCOL_FRAME protFrame_send);
int32_t ReceDataStructUnpack(uint8_t *u8ReceBuff, MBTCP_PROTOCOL_FRAME * protFrame_rece);

int F28_floatDataHandle_X100(float data);
int F28_floatDataHandle_X10(float data);
int F28_floatDataHandle_X1000(float data);

#endif



