#include "HartComTask.h"
#include "FML_hartDataProcess.h"
#include "rs232.h"
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "key.h"


/* hart protol */
StHartDataFrame stHartFrameRecv;    //hart帧接收处理结构体
StHartDataFrame stHartFrameSend;    //hart帧发送处理结构体

static u32 rs232RxIndex = 0;
float diffGage = 0;

/* other parameters*/
const uint8_t u8HartCMD_0[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x02, 0x80, 0x00, 0x00, 0x82};  //短帧通用命令0：读标识码
const uint8_t u8HartCMD_1[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x82, 026, 0x06, 0x13, 0x96, 0xDD, 0x03, 0x00, 0xF9};


/* 十六进制转十进制浮点数 */
float IEEE754Int2Float(int x)
{
	return *(float *)&x;
}

/* 十进制浮点数转十六进制 */
int IEEE754Float2Int(float x)
{
	return *(int *)&x;
}

/**
  * 函数功能：将串口接收到的1字节数据放入数据缓冲区
  * 输入参数：u8Data 数据
  * 返 回 值：无
  * 说    明：自定义串口接收中断使用
  */
void rs232RecvData(u8 u8Data)
{
	if(rs232RxIndex == RECEVBUFF_SIZE)
	{
		rs232RxIndex = 0;
	}
	rs232_RxBuf[rs232RxIndex++] = u8Data;
}

/**
  * 函数功能：HART数据帧发送函数
  * 输入参数：pData 数据
  *          size 数据长度
  * 返 回 值：无
  * 说    明：自定义串口接收中断使用
  */
void HartDataSend(uint8_t *pData, uint8_t size)
{
	rs232_send_data(pData, size);
}

/**
  * 函数功能：HART通信线程
  * 输入参数：pArgs 线程参数 
  * 返 回 值：无
  * 说    明：
  */
void hartCommunicate_thread(void * pArgs)
{ 
	int i = 0;
	int k = 0;
	u8 key;
	int32_t s32TxLength = 0;
	int32_t s32SaveErr = 0;
	int32_t s32RecvLen = 0;
	uint8_t manufacturerID = 0;
	uint8_t deviceType = 0;
	uint8_t deviceID[3];
	uint8_t u8RecvCompleteFlag = 0;
	uint32_t rs232RxIndexLast = 0;
	int u32MainValue = 0;
	/* 备份接收写指针 */
	rs232RxIndexLast = rs232RxIndex;
#if 1
	/* hart数字信号发送测试：作为主机发送短帧（相当于发送u8HartCMD_0) */
	memset(&stHartFrameSend, 0, sizeof(stHartFrameSend));
	stHartFrameSend.u8Start = TYPE_MAIN_SLAVE_SHORT_FRAME;
	stHartFrameSend.u8Addr[0] = 0x80;
	stHartFrameSend.u8Command = 0x00;
	stHartFrameSend.u8Bcnt = 0x00;
	s32TxLength = SendDataStructProcess(rs232_TxBuf, stHartFrameSend);
#endif
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY0_PRES)//KEY0按下,发送一次数据
		{
			HartDataSend(rs232_TxBuf, s32TxLength);
		}
		//检测到接收缓冲区有数据 

		if(rs232RxIndex > 0)
		{
			if(rs232RxIndexLast != rs232RxIndex)
			{
				rs232RxIndexLast = rs232RxIndex;
				vTaskDelay(10);
			}
			// 10ms没有收到新数据或者填满25字节则认为一帧ok 
			else
			{
				u8RecvCompleteFlag = 1;
			}  
		}
		else
		{
			vTaskDelay(10);
		}
		if(u8RecvCompleteFlag == 1)
		{
			// 获取接收缓冲区的长度 
			s32RecvLen = rs232RxIndex;
			rs232RxIndex = 0;
			// 备份接收指针 
			u8RecvCompleteFlag = 0;
			rs232RxIndexLast = rs232RxIndex;
			
			printf("%d\r\n", s32RecvLen);
			
			for(; i<s32RecvLen; i++)
			{
				printf("%d\r\n", rs232_RxBuf[i]);
			}

			memset(&stHartFrameRecv, 0 ,sizeof(StHartDataFrame));
			s32SaveErr = RecvDataStructProcess(rs232_RxBuf, s32RecvLen, &stHartFrameRecv);
			printf("s32SaveErr: %d\r\n", s32SaveErr);
			if(s32SaveErr == 0)
			{	
				manufacturerID = stHartFrameRecv.u8Data[1];
				printf("manufacturerID: %d\r\n", manufacturerID);
				deviceType = stHartFrameRecv.u8Data[2];
				printf("deviceType: %d\r\n", deviceType);
				memcpy(deviceID, stHartFrameRecv.u8Data + (stHartFrameRecv.u8Bcnt - 5), 3);
				
				for(; k<3; k++)
				{
					printf("%d\r\n", deviceID[k]);
				}

				memset(&stHartFrameSend, 0, sizeof(stHartFrameSend));
				stHartFrameSend.u8Start = TYPE_MAIN_SLAVE_LONG_FRAME;
				stHartFrameSend.u8Addr[0] = manufacturerID;
				stHartFrameSend.u8Addr[1] = deviceType;
				memcpy(stHartFrameSend.u8Addr + 2, deviceID, 3);
				stHartFrameSend.u8Command = 0x03;
				stHartFrameSend.u8Bcnt = 0x00;
				s32TxLength = SendDataStructProcess(rs232_TxBuf, stHartFrameSend);
				printf("s32TxLength = %d\r\n", s32TxLength);
				if(s32TxLength != -1)
				{
					while(1)
					{
						// 向loop中发送hart帧 
						HartDataSend(rs232_TxBuf, s32TxLength);
						do
						{
							if(rs232RxIndex > 0)
							{
								if(rs232RxIndexLast != rs232RxIndex)
								{
									rs232RxIndexLast = rs232RxIndex;
									vTaskDelay(10);
								}
								// 5ms没有收到新数据或者填满25字节则认为一帧ok 
								else
								{
									u8RecvCompleteFlag = 1;
								}  
							}
							else
							{
								vTaskDelay(10);
							}
						}while (u8RecvCompleteFlag != 1); 
						if(u8RecvCompleteFlag == 1)
						{
							// 获取接收缓冲区的长度 
							s32RecvLen = rs232RxIndex;
							rs232RxIndex = 0;
							// 备份接收指针 
							u8RecvCompleteFlag = 0;
							rs232RxIndexLast = rs232RxIndex;

							memset(&stHartFrameRecv, 0 ,sizeof(StHartDataFrame));
							s32SaveErr = RecvDataStructProcess(rs232_RxBuf, s32RecvLen, &stHartFrameRecv);
							if(s32SaveErr == 0)
							{
								switch(stHartFrameRecv.u8Start)
								{
									case TYPE_MAIN_SLAVE_SHORT_FRAME:
										break;
									case TYPE_MAIN_SLAVE_LONG_FRAME:
										break;
									case TYPE_SLAVE_MAIN_SHORT_FRAME:
									case TYPE_SLAVE_MAIN_LONG_FRAME:
										switch (stHartFrameRecv.u8Command)
										{
											// 1#命令，读主变量(pv) 
											case 1:  
												u32MainValue = stHartFrameRecv.u8Data[1] << 24;
												u32MainValue |= stHartFrameRecv.u8Data[2] << 16;
												u32MainValue |= stHartFrameRecv.u8Data[3] << 8;
												u32MainValue |= stHartFrameRecv.u8Data[4];
												diffGage = 1000 * IEEE754Int2Float(u32MainValue);
												printf("diffgage: %f pa\t%d\r\n", diffGage, stHartFrameRecv.u8Data[0]);
												break;
											// 3#命令，读动态变量与主变量电流值 
											case 3:
												u32MainValue = stHartFrameRecv.u8Data[5] << 24;
												u32MainValue |= stHartFrameRecv.u8Data[6] << 16;
												u32MainValue |= stHartFrameRecv.u8Data[7] << 8;
												u32MainValue |= stHartFrameRecv.u8Data[8];
												diffGage = 1000 * IEEE754Int2Float(u32MainValue);
												printf("diffgage: %f pa\t%d\r\n", diffGage, stHartFrameRecv.u8Data[4]);
												break;
											default:
												break;
										}
										break;
									default:
										break;      
								}
							}
						}
					}
				}
			}
		} 
	}
}

