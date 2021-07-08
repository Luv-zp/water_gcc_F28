#include <stdio.h>
#include <string.h>
#include "FML_hartDataProcess.h"




/**
  * 函数功能：hart数据帧CHK校验
  * 输入参数：u8Data 需要校验的数据
             length 所校验的数据长度
  * 返 回 值：CHK校验值
  */
uint8_t HartDataCHK(uint8_t *u8Data, uint16_t length)
{
	uint8_t i = 0;
	uint8_t u8ChkData = u8Data[0];
	for(i = 1; i < length; i++)
	{
		u8ChkData ^= u8Data[i];
	}
	return u8ChkData;
}

/**
  * 函数功能：接收到的HART数据包处理
  * 输入参数：u8Data:需要校验的数据
             size:校验的数据长度
             stHart:存储Hart数据帧数据
  * 返 回 值：0 数据存储OK， -1 失败
  */
int32_t RecvDataStructProcess(uint8_t *u8Data, uint16_t size, StHartDataFrame *stHart)
{
	int32_t s32Ret = 0;
	uint16_t i = 0;
	uint16_t u16Size = 0;
	uint8_t u8Start = 0;
	uint8_t u8SynFlag = 0;
	uint8_t *u8TmpData = NULL;

	if((u8Data == NULL) || (stHart == NULL))
	{
		return -1;
	}
	/* 处理5-20个同步头0xFF */
	for(i = 0; i < size; i++)
	{
		if(u8Data[i] == 0xFF)
		{
			u8SynFlag = 1;
		}
		if((u8SynFlag == 1) && (u8Data[i] != 0xFF))
		{
			break;
		}
	}
	/* 除去同步0xFF后的字节数 */
	u16Size = size - i;
	u8TmpData = u8Data + i;
	u8Start = u8TmpData[0];
	/* 奇偶校验 */
	if(HartDataCHK(u8TmpData, u16Size -2) != u8TmpData[u16Size - 2])
	{
		printf("%d\r\n",u8TmpData[u16Size - 1]);
		return -1;
	}
	switch(u8Start)
	{
		/* 主机到从机短帧结构(角色：从机) */
		case TYPE_MAIN_SLAVE_SHORT_FRAME: 
			stHart->u8Start = u8TmpData[0];     //定界符 1byte
			stHart->u8Addr[0] = u8TmpData[1];   //地址 1byte
			stHart->u8Command = u8TmpData[2];   //命令 1byte
			stHart->u8Bcnt = u8TmpData[3];      //长度 1byte
			memcpy(stHart->u8Data, u8TmpData + 4, stHart->u8Bcnt);   //数据
			stHart->u8Parity = u8TmpData[4 + stHart->u8Bcnt];   //奇偶校验
			break;
		/* 主机到从机长帧结构(角色：从机) */
		case TYPE_MAIN_SLAVE_LONG_FRAME:  
			stHart->u8Start = u8TmpData[0];     //定界符 1byte
			memcpy(stHart->u8Addr, u8TmpData + 1, 5);   //地址 5bytes
			stHart->u8Command = u8TmpData[6];   //命令 1byte
			stHart->u8Bcnt = u8TmpData[7];      //长度 1byte
			memcpy(stHart->u8Data, u8TmpData + 8, stHart->u8Bcnt);   //数据
			stHart->u8Parity = u8TmpData[8 + stHart->u8Bcnt];   //奇偶校验
			break;
		/* 从机到主机短帧结构(角色：主机) */
		case TYPE_SLAVE_MAIN_SHORT_FRAME:
			stHart->u8Start = u8TmpData[0];     //定界符 1byte
			stHart->u8Addr[0] = u8TmpData[1];   //地址 1byte
			stHart->u8Command = u8TmpData[2];   //命令 1byte
			stHart->u8Bcnt = u8TmpData[3];      //长度 1byte
			memcpy(stHart->u8Status, u8TmpData + 4, 2);   //响应码 2bytes
			memcpy(stHart->u8Data, u8TmpData + 6, stHart->u8Bcnt);   //数据
			stHart->u8Parity = u8TmpData[4 + stHart->u8Bcnt];   //奇偶校验
			break;
		/* 从机到主机长帧结构(角色：主机) */
		case TYPE_SLAVE_MAIN_LONG_FRAME:
			stHart->u8Start = u8TmpData[0];     //定界符 1byte
			memcpy(stHart->u8Addr, u8TmpData + 1, 5);   //地址 5bytes
			stHart->u8Command = u8TmpData[6];   //命令 1byte
			stHart->u8Bcnt = u8TmpData[7];      //长度 1byte
			memcpy(stHart->u8Status, u8TmpData + 8, 2);   //响应码 2bytes
			memcpy(stHart->u8Data, u8TmpData + 10, stHart->u8Bcnt);   //数据
			stHart->u8Parity = u8TmpData[8 + stHart->u8Bcnt];   //奇偶校验
			break;
		/* 突发模式短帧结构(暂时预留，同从主短帧结构)[角色：主机，接收从机突发数据] */
		case TYPE_BURST_MODE_SHORT_FRAME:
			break;
		/* 突发模式短帧结构(暂时预留，同从主长帧结构)[角色：主机，接收从机突发数据] */
		case TYPE_BURST_MODE_LONG_FRAME:
			break;
		default:
			s32Ret = -1;
			break;
	}
	return s32Ret;
}

/**
  * 函数功能：将数据进行打包处理
  * 输入参数：u8Data 拼包成功后需要发送的数据
  *          stHart 入口Hart参数
  * 返 回 值：>0 数据长度， -1 拼包失败
  */
int32_t SendDataStructProcess(uint8_t *u8Data, StHartDataFrame stHart)
{
	int32_t s32Size = -1;
	if(u8Data == NULL)
	{
		return -1;
	}
	/* 注意角色应该和接收预处理相反 */
	switch(stHart.u8Start)
	{
		/* 主机到从机短帧结构(角色：主机) */
		case TYPE_MAIN_SLAVE_SHORT_FRAME:
			memset(u8Data, 0xFF, 5);        //同步头：5-20个0xFF，通常采用5个
			u8Data[5] = stHart.u8Start;     //定界符: 0x02
			u8Data[6] = stHart.u8Addr[0];   //地址： 短帧1byte
			u8Data[7] = stHart.u8Command;   //命令
			u8Data[8] = stHart.u8Bcnt;      //数据长度： 最大25个字节
			memcpy(u8Data + 9, stHart.u8Data, stHart.u8Bcnt);     //数据
			u8Data[9 + stHart.u8Bcnt] = HartDataCHK(u8Data + 5, 4 + stHart.u8Bcnt); //CHK校验
			s32Size = 9 + stHart.u8Bcnt + 1;      //整包数据长度
			break;
		/* 主机到从机长帧结构(角色：主机) */
		case TYPE_MAIN_SLAVE_LONG_FRAME:
			memset(u8Data, 0xFF, 5);        //同步头：5-20个0xFF，通常采用5个
			u8Data[5] = stHart.u8Start;     //定界符: 0x82
			memcpy(u8Data + 6, stHart.u8Addr, 5);   //地址： 长帧5bytes
			u8Data[11] = stHart.u8Command;   //命令
			u8Data[12] = stHart.u8Bcnt;      //数据长度： 最大25个字节
			memcpy(u8Data + 13, stHart.u8Data, stHart.u8Bcnt);     //数据
			u8Data[13 + stHart.u8Bcnt] = HartDataCHK(u8Data + 5, 8 + stHart.u8Bcnt); //CHK校验
			s32Size = 13 + stHart.u8Bcnt + 1;      //整包数据长度
			break;
		/* 从机到主机短帧结构(角色：从机) */
		case TYPE_SLAVE_MAIN_SHORT_FRAME:
			memset(u8Data, 0xFF, 5);        //同步头：5-20个0xFF，通常采用5个
			u8Data[5] = stHart.u8Start;     //定界符: 0x02
			u8Data[6] = stHart.u8Addr[0];   //地址： 短帧1byte
			u8Data[7] = stHart.u8Command;   //命令
			u8Data[8] = stHart.u8Bcnt;      //数据长度： 最大27个字节
			memcpy(u8Data + 9, stHart.u8Status, 2);   //响应码：2bytes
			memcpy(u8Data + 11, stHart.u8Data, stHart.u8Bcnt - 2);     //数据
			u8Data[9 + stHart.u8Bcnt] = HartDataCHK(u8Data + 5, 4 + stHart.u8Bcnt); //CHK校验
			s32Size = 9 + stHart.u8Bcnt + 1;      //整包数据长度
			break;
		/* 从机到主机长帧结构(角色：从机) */
		case TYPE_SLAVE_MAIN_LONG_FRAME:
			memset(u8Data, 0xFF, 5);        //同步头：5-20个0xFF，通常采用5个
			u8Data[5] = stHart.u8Start;     //定界符: 0x02
			memcpy(u8Data + 6, stHart.u8Addr, 5);   //地址： 长帧5bytes
			u8Data[11] = stHart.u8Command;   //命令
			u8Data[12] = stHart.u8Bcnt;      //数据长度： 最大27个字节
			memcpy(u8Data + 13, stHart.u8Status, 2);   //响应码：2bytes
			memcpy(u8Data + 15, stHart.u8Data, stHart.u8Bcnt - 2);     //数据
			u8Data[13 + stHart.u8Bcnt] = HartDataCHK(u8Data + 5, 8 + stHart.u8Bcnt); //CHK校验
			s32Size = 13 + stHart.u8Bcnt + 1;      //整包数据长度
			break;
		/* 突发模式短帧结构(角色：从机)[只有从机可以处于突发模式] */
		case TYPE_BURST_MODE_SHORT_FRAME:
			break;
		/* 突发模式长帧结构(角色：从机)[只有从机可以处于突发模式] */
		case TYPE_BURST_MODE_LONG_FRAME:
			break;
		default:
			s32Size = -1;
		break;
	}
	return s32Size;
}
  
