#include <stdio.h>
#include <string.h>
#include "FML_hartDataProcess.h"




/**
  * �������ܣ�hart����֡CHKУ��
  * ���������u8Data ��ҪУ�������
             length ��У������ݳ���
  * �� �� ֵ��CHKУ��ֵ
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
  * �������ܣ����յ���HART���ݰ�����
  * ���������u8Data:��ҪУ�������
             size:У������ݳ���
             stHart:�洢Hart����֡����
  * �� �� ֵ��0 ���ݴ洢OK�� -1 ʧ��
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
	/* ����5-20��ͬ��ͷ0xFF */
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
	/* ��ȥͬ��0xFF����ֽ��� */
	u16Size = size - i;
	u8TmpData = u8Data + i;
	u8Start = u8TmpData[0];
	/* ��żУ�� */
	if(HartDataCHK(u8TmpData, u16Size -2) != u8TmpData[u16Size - 2])
	{
		printf("%d\r\n",u8TmpData[u16Size - 1]);
		return -1;
	}
	switch(u8Start)
	{
		/* �������ӻ���֡�ṹ(��ɫ���ӻ�) */
		case TYPE_MAIN_SLAVE_SHORT_FRAME: 
			stHart->u8Start = u8TmpData[0];     //����� 1byte
			stHart->u8Addr[0] = u8TmpData[1];   //��ַ 1byte
			stHart->u8Command = u8TmpData[2];   //���� 1byte
			stHart->u8Bcnt = u8TmpData[3];      //���� 1byte
			memcpy(stHart->u8Data, u8TmpData + 4, stHart->u8Bcnt);   //����
			stHart->u8Parity = u8TmpData[4 + stHart->u8Bcnt];   //��żУ��
			break;
		/* �������ӻ���֡�ṹ(��ɫ���ӻ�) */
		case TYPE_MAIN_SLAVE_LONG_FRAME:  
			stHart->u8Start = u8TmpData[0];     //����� 1byte
			memcpy(stHart->u8Addr, u8TmpData + 1, 5);   //��ַ 5bytes
			stHart->u8Command = u8TmpData[6];   //���� 1byte
			stHart->u8Bcnt = u8TmpData[7];      //���� 1byte
			memcpy(stHart->u8Data, u8TmpData + 8, stHart->u8Bcnt);   //����
			stHart->u8Parity = u8TmpData[8 + stHart->u8Bcnt];   //��żУ��
			break;
		/* �ӻ���������֡�ṹ(��ɫ������) */
		case TYPE_SLAVE_MAIN_SHORT_FRAME:
			stHart->u8Start = u8TmpData[0];     //����� 1byte
			stHart->u8Addr[0] = u8TmpData[1];   //��ַ 1byte
			stHart->u8Command = u8TmpData[2];   //���� 1byte
			stHart->u8Bcnt = u8TmpData[3];      //���� 1byte
			memcpy(stHart->u8Status, u8TmpData + 4, 2);   //��Ӧ�� 2bytes
			memcpy(stHart->u8Data, u8TmpData + 6, stHart->u8Bcnt);   //����
			stHart->u8Parity = u8TmpData[4 + stHart->u8Bcnt];   //��żУ��
			break;
		/* �ӻ���������֡�ṹ(��ɫ������) */
		case TYPE_SLAVE_MAIN_LONG_FRAME:
			stHart->u8Start = u8TmpData[0];     //����� 1byte
			memcpy(stHart->u8Addr, u8TmpData + 1, 5);   //��ַ 5bytes
			stHart->u8Command = u8TmpData[6];   //���� 1byte
			stHart->u8Bcnt = u8TmpData[7];      //���� 1byte
			memcpy(stHart->u8Status, u8TmpData + 8, 2);   //��Ӧ�� 2bytes
			memcpy(stHart->u8Data, u8TmpData + 10, stHart->u8Bcnt);   //����
			stHart->u8Parity = u8TmpData[8 + stHart->u8Bcnt];   //��żУ��
			break;
		/* ͻ��ģʽ��֡�ṹ(��ʱԤ����ͬ������֡�ṹ)[��ɫ�����������մӻ�ͻ������] */
		case TYPE_BURST_MODE_SHORT_FRAME:
			break;
		/* ͻ��ģʽ��֡�ṹ(��ʱԤ����ͬ������֡�ṹ)[��ɫ�����������մӻ�ͻ������] */
		case TYPE_BURST_MODE_LONG_FRAME:
			break;
		default:
			s32Ret = -1;
			break;
	}
	return s32Ret;
}

/**
  * �������ܣ������ݽ��д������
  * ���������u8Data ƴ���ɹ�����Ҫ���͵�����
  *          stHart ���Hart����
  * �� �� ֵ��>0 ���ݳ��ȣ� -1 ƴ��ʧ��
  */
int32_t SendDataStructProcess(uint8_t *u8Data, StHartDataFrame stHart)
{
	int32_t s32Size = -1;
	if(u8Data == NULL)
	{
		return -1;
	}
	/* ע���ɫӦ�úͽ���Ԥ�����෴ */
	switch(stHart.u8Start)
	{
		/* �������ӻ���֡�ṹ(��ɫ������) */
		case TYPE_MAIN_SLAVE_SHORT_FRAME:
			memset(u8Data, 0xFF, 5);        //ͬ��ͷ��5-20��0xFF��ͨ������5��
			u8Data[5] = stHart.u8Start;     //�����: 0x02
			u8Data[6] = stHart.u8Addr[0];   //��ַ�� ��֡1byte
			u8Data[7] = stHart.u8Command;   //����
			u8Data[8] = stHart.u8Bcnt;      //���ݳ��ȣ� ���25���ֽ�
			memcpy(u8Data + 9, stHart.u8Data, stHart.u8Bcnt);     //����
			u8Data[9 + stHart.u8Bcnt] = HartDataCHK(u8Data + 5, 4 + stHart.u8Bcnt); //CHKУ��
			s32Size = 9 + stHart.u8Bcnt + 1;      //�������ݳ���
			break;
		/* �������ӻ���֡�ṹ(��ɫ������) */
		case TYPE_MAIN_SLAVE_LONG_FRAME:
			memset(u8Data, 0xFF, 5);        //ͬ��ͷ��5-20��0xFF��ͨ������5��
			u8Data[5] = stHart.u8Start;     //�����: 0x82
			memcpy(u8Data + 6, stHart.u8Addr, 5);   //��ַ�� ��֡5bytes
			u8Data[11] = stHart.u8Command;   //����
			u8Data[12] = stHart.u8Bcnt;      //���ݳ��ȣ� ���25���ֽ�
			memcpy(u8Data + 13, stHart.u8Data, stHart.u8Bcnt);     //����
			u8Data[13 + stHart.u8Bcnt] = HartDataCHK(u8Data + 5, 8 + stHart.u8Bcnt); //CHKУ��
			s32Size = 13 + stHart.u8Bcnt + 1;      //�������ݳ���
			break;
		/* �ӻ���������֡�ṹ(��ɫ���ӻ�) */
		case TYPE_SLAVE_MAIN_SHORT_FRAME:
			memset(u8Data, 0xFF, 5);        //ͬ��ͷ��5-20��0xFF��ͨ������5��
			u8Data[5] = stHart.u8Start;     //�����: 0x02
			u8Data[6] = stHart.u8Addr[0];   //��ַ�� ��֡1byte
			u8Data[7] = stHart.u8Command;   //����
			u8Data[8] = stHart.u8Bcnt;      //���ݳ��ȣ� ���27���ֽ�
			memcpy(u8Data + 9, stHart.u8Status, 2);   //��Ӧ�룺2bytes
			memcpy(u8Data + 11, stHart.u8Data, stHart.u8Bcnt - 2);     //����
			u8Data[9 + stHart.u8Bcnt] = HartDataCHK(u8Data + 5, 4 + stHart.u8Bcnt); //CHKУ��
			s32Size = 9 + stHart.u8Bcnt + 1;      //�������ݳ���
			break;
		/* �ӻ���������֡�ṹ(��ɫ���ӻ�) */
		case TYPE_SLAVE_MAIN_LONG_FRAME:
			memset(u8Data, 0xFF, 5);        //ͬ��ͷ��5-20��0xFF��ͨ������5��
			u8Data[5] = stHart.u8Start;     //�����: 0x02
			memcpy(u8Data + 6, stHart.u8Addr, 5);   //��ַ�� ��֡5bytes
			u8Data[11] = stHart.u8Command;   //����
			u8Data[12] = stHart.u8Bcnt;      //���ݳ��ȣ� ���27���ֽ�
			memcpy(u8Data + 13, stHart.u8Status, 2);   //��Ӧ�룺2bytes
			memcpy(u8Data + 15, stHart.u8Data, stHart.u8Bcnt - 2);     //����
			u8Data[13 + stHart.u8Bcnt] = HartDataCHK(u8Data + 5, 8 + stHart.u8Bcnt); //CHKУ��
			s32Size = 13 + stHart.u8Bcnt + 1;      //�������ݳ���
			break;
		/* ͻ��ģʽ��֡�ṹ(��ɫ���ӻ�)[ֻ�дӻ����Դ���ͻ��ģʽ] */
		case TYPE_BURST_MODE_SHORT_FRAME:
			break;
		/* ͻ��ģʽ��֡�ṹ(��ɫ���ӻ�)[ֻ�дӻ����Դ���ͻ��ģʽ] */
		case TYPE_BURST_MODE_LONG_FRAME:
			break;
		default:
			s32Size = -1;
		break;
	}
	return s32Size;
}
  
