#ifndef _F28_MB_TCP_PROT_H
#define _F28_MB_TCP_PROT_H
#include "sys.h"

//����mbtcp����֡����ͷ
#define SET_PARA_TID		0x0001		//�����ʶ��
#define SET_PARA_PID		0x0000		//Э���ʶ��

#define READ_PARA_TID		0x0011		//�����ʶ��
#define READ_PARA_PID		0x0000		//Э���ʶ��

#define START_CIRCLE_TID	0x0002		//�����ʶ��
#define START_CIRCLE_PID	0x0000		//Э���ʶ��

#define READ_REALTIME_TID	0x0003		//�����ʶ�� ���������ʶ��
#define READ_REALTIME_PID	0x0000		//Э���ʶ�� ��0000��ʶmodbus tcpЭ�飩

#define READ_LAST_TID		0x0004		//�����ʶ��
#define READ_LAST_PID		0x0000		//Э���ʶ��

#define STOP_CIRCLE_TID		0x0005		//�����ʶ��
#define STOP_CIRCLE_PID		0x0000		//Э���ʶ��

//������
#define TYPE_READ_HOLDING	0x03	//�����ּĴ���
#define TYPE_WRITE_HOLDING	0x10	//д���ּĴ���	


//���͡��������ݻ���������
#define F28_RxBufSize 200
#define F28_TxBufSize 200

typedef struct 
{
	u16 u16TID;					//�����ʶ��
	u16 u16PID;					//Э���ʶ��
	u16 u16ByteLen;				//�ֽڳ���
	u8 u8UID;					//��������ʶ
	u8 u8FunCode;				//������
	u16 u16RegStartingAddr;		//�Ĵ�����ʼ��ַ
	u16 u16RegCount;			//�Ĵ������� 
	u8 u8ByteCount;				//���ݶ��ֽ���
	u8 u8DataSeg[100];				//���ݶ�	 �������ָ��
}MBTCP_PROTOCOL_FRAME;

extern u8 F28_ReceBuf[F28_RxBufSize];
extern u8 F28_SendBuf[F28_TxBufSize];
int32_t SendDataStructPack(uint8_t *u8SendBuff, MBTCP_PROTOCOL_FRAME protFrame_send);
int32_t ReceDataStructUnpack(uint8_t *u8ReceBuff, MBTCP_PROTOCOL_FRAME * protFrame_rece);

int F28_floatDataHandle_X100(float data);
int F28_floatDataHandle_X10(float data);
int F28_floatDataHandle_X1000(float data);

#endif



