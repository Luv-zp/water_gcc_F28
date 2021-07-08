#ifndef MB_TCP_PROTOCOL_H
#define MB_TCP_PROTOCOL_H
#include "typedef.h"

//定义mbtcp数据帧报文头
#define SET_PARA_TID	0x0001		//传输标识符
#define SET_PARA_PID	0x0000		//协议标识符

#define READ_PARA_TID	0x0011		//传输标识符
#define READ_PARA_PID	0x0000		//协议标识符

#define START_CIRCLE_TID	0x0002		//传输标识符
#define START_CIRCLE_PID	0x0000		//协议标识符

#define READ_REALTIME_TID	0x0003		//传输标识符 （事务处理标识）
#define READ_REALTIME_PID	0x0000		//协议标识符 （0000标识modbus tcp协议）

#define READ_LAST_TID	0x0004		//传输标识符
#define READ_LAST_PID	0x0000		//协议标识符

#define STOP_CIRCLE_TID	0x0005		//传输标识符
#define STOP_CIRCLE_PID	0x0000		//协议标识符

//功能码
#define TYPE_READ_HOLDING	0x03	//读保持寄存器
#define TYPE_WRITE_HOLDING	0x10	//写保持寄存器

typedef struct MBTCP_PROTOCOL_FRAME
{
	u16 u16TID;					//传输标识符
	u16 u16PID;					//协议标识符
	u16 u16ByteLen;				//字节长度
	u8 u8UID;					//服务器标识
	u8 u8FunCode;				//功能码
	u16 u16RegStartingAddr;		//寄存器起始地址
	u16 u16RegCount;			//寄存器个数 
	u8 u8ByteCount;				//数据段字节数
	u8 * u8DataSeg;				//数据段	
}MBTCP_PROTOCOL_FRAME;

int32_t SendDataStructPack(uint8_t *u8SendBuff, MBTCP_PROTOCOL_FRAME protFrame_send);
int32_t ReceDataStructUnpack(uint8_t *u8ReceBuff, MBTCP_PROTOCOL_FRAME protFrame_rece);


#endif /*MB_TCP_PROTOCOL_H*/

