#ifndef _FML_HARTDATAPROCESS_H
#define _FML_HARTDATAPROCESS_H

#include "sys.h"


typedef struct _tagStHartDataFrame
{
#if 0
	uint8_t u8Preamble[20];   //前导符：作为通信同步的需要，5-20字节的0xFF,通常采用5个字节
#endif
	uint8_t u8Start;      //定界符：主->从(短帧0x02,长帧0x82) 从->主(短帧0x06, 长帧0x86) 突发(0x01/0x81)
	uint8_t u8Addr[5];    //地址：短帧结构1个字节，长帧结构5个字节
	uint8_t u8Command;    //命令：范围为253个。用hex表示，31、127、254、255为预留值
	uint8_t u8Bcnt;       //字节长度：从该字节下一个字节到最后的字节数(不包括校验字节数)
	uint8_t u8Status[2];  //响应码：只存在于从机相应主机消息的时候
	uint8_t u8Data[25];   //数据：最多25字节
	uint8_t u8Parity;     //校验码：奇偶纵向校验，从起始字节到就校验前一字节为止
}StHartDataFrame;

typedef enum _tagEmHartFrameType
{
	TYPE_MAIN_SLAVE_SHORT_FRAME = 0x02,   //主机到从机短帧结构
	TYPE_MAIN_SLAVE_LONG_FRAME = 0x82,    //主机到从机长帧结构
	TYPE_SLAVE_MAIN_SHORT_FRAME = 0x06,   //从机到主机短帧结构
	TYPE_SLAVE_MAIN_LONG_FRAME = 0x86,    //从机到主机长帧结构
	TYPE_BURST_MODE_SHORT_FRAME = 0x01,   //突发模式短帧结构
	TYPE_BURST_MODE_LONG_FRAME = 0X81,    //突发模式长帧结构
}EmHartFrameType;

int32_t RecvDataStructProcess(uint8_t *u8Data, uint16_t size, StHartDataFrame *stHart);
int32_t SendDataStructProcess(uint8_t *u8Data, StHartDataFrame stHart);

#endif


