#include "HartComTask.h"
#include "FML_hartDataProcess.h"
#include "rs232.h"
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "key.h"


/* hart protol */
StHartDataFrame stHartFrameRecv;    //hart֡���մ���ṹ��
StHartDataFrame stHartFrameSend;    //hart֡���ʹ���ṹ��

static u32 rs232RxIndex = 0;
float diffGage = 0;

/* other parameters*/
const uint8_t u8HartCMD_0[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x02, 0x80, 0x00, 0x00, 0x82};  //��֡ͨ������0������ʶ��
const uint8_t u8HartCMD_1[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x82, 026, 0x06, 0x13, 0x96, 0xDD, 0x03, 0x00, 0xF9};


/* ʮ������תʮ���Ƹ����� */
float IEEE754Int2Float(int x)
{
	return *(float *)&x;
}

/* ʮ���Ƹ�����תʮ������ */
int IEEE754Float2Int(float x)
{
	return *(int *)&x;
}

/**
  * �������ܣ������ڽ��յ���1�ֽ����ݷ������ݻ�����
  * ���������u8Data ����
  * �� �� ֵ����
  * ˵    �����Զ��崮�ڽ����ж�ʹ��
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
  * �������ܣ�HART����֡���ͺ���
  * ���������pData ����
  *          size ���ݳ���
  * �� �� ֵ����
  * ˵    �����Զ��崮�ڽ����ж�ʹ��
  */
void HartDataSend(uint8_t *pData, uint8_t size)
{
	rs232_send_data(pData, size);
}

/**
  * �������ܣ�HARTͨ���߳�
  * ���������pArgs �̲߳��� 
  * �� �� ֵ����
  * ˵    ����
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
	/* ���ݽ���дָ�� */
	rs232RxIndexLast = rs232RxIndex;
#if 1
	/* hart�����źŷ��Ͳ��ԣ���Ϊ�������Ͷ�֡���൱�ڷ���u8HartCMD_0) */
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
		if(key==KEY0_PRES)//KEY0����,����һ������
		{
			HartDataSend(rs232_TxBuf, s32TxLength);
		}
		//��⵽���ջ����������� 

		if(rs232RxIndex > 0)
		{
			if(rs232RxIndexLast != rs232RxIndex)
			{
				rs232RxIndexLast = rs232RxIndex;
				vTaskDelay(10);
			}
			// 10msû���յ������ݻ�������25�ֽ�����Ϊһ֡ok 
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
			// ��ȡ���ջ������ĳ��� 
			s32RecvLen = rs232RxIndex;
			rs232RxIndex = 0;
			// ���ݽ���ָ�� 
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
						// ��loop�з���hart֡ 
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
								// 5msû���յ������ݻ�������25�ֽ�����Ϊһ֡ok 
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
							// ��ȡ���ջ������ĳ��� 
							s32RecvLen = rs232RxIndex;
							rs232RxIndex = 0;
							// ���ݽ���ָ�� 
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
											// 1#�����������(pv) 
											case 1:  
												u32MainValue = stHartFrameRecv.u8Data[1] << 24;
												u32MainValue |= stHartFrameRecv.u8Data[2] << 16;
												u32MainValue |= stHartFrameRecv.u8Data[3] << 8;
												u32MainValue |= stHartFrameRecv.u8Data[4];
												diffGage = 1000 * IEEE754Int2Float(u32MainValue);
												printf("diffgage: %f pa\t%d\r\n", diffGage, stHartFrameRecv.u8Data[0]);
												break;
											// 3#�������̬����������������ֵ 
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

