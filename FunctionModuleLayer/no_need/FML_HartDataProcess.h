#ifndef _FML_HARTDATAPROCESS_H
#define _FML_HARTDATAPROCESS_H

#include "sys.h"


typedef struct _tagStHartDataFrame
{
#if 0
	uint8_t u8Preamble[20];   //ǰ��������Ϊͨ��ͬ������Ҫ��5-20�ֽڵ�0xFF,ͨ������5���ֽ�
#endif
	uint8_t u8Start;      //���������->��(��֡0x02,��֡0x82) ��->��(��֡0x06, ��֡0x86) ͻ��(0x01/0x81)
	uint8_t u8Addr[5];    //��ַ����֡�ṹ1���ֽڣ���֡�ṹ5���ֽ�
	uint8_t u8Command;    //�����ΧΪ253������hex��ʾ��31��127��254��255ΪԤ��ֵ
	uint8_t u8Bcnt;       //�ֽڳ��ȣ��Ӹ��ֽ���һ���ֽڵ������ֽ���(������У���ֽ���)
	uint8_t u8Status[2];  //��Ӧ�룺ֻ�����ڴӻ���Ӧ������Ϣ��ʱ��
	uint8_t u8Data[25];   //���ݣ����25�ֽ�
	uint8_t u8Parity;     //У���룺��ż����У�飬����ʼ�ֽڵ���У��ǰһ�ֽ�Ϊֹ
}StHartDataFrame;

typedef enum _tagEmHartFrameType
{
	TYPE_MAIN_SLAVE_SHORT_FRAME = 0x02,   //�������ӻ���֡�ṹ
	TYPE_MAIN_SLAVE_LONG_FRAME = 0x82,    //�������ӻ���֡�ṹ
	TYPE_SLAVE_MAIN_SHORT_FRAME = 0x06,   //�ӻ���������֡�ṹ
	TYPE_SLAVE_MAIN_LONG_FRAME = 0x86,    //�ӻ���������֡�ṹ
	TYPE_BURST_MODE_SHORT_FRAME = 0x01,   //ͻ��ģʽ��֡�ṹ
	TYPE_BURST_MODE_LONG_FRAME = 0X81,    //ͻ��ģʽ��֡�ṹ
}EmHartFrameType;

int32_t RecvDataStructProcess(uint8_t *u8Data, uint16_t size, StHartDataFrame *stHart);
int32_t SendDataStructProcess(uint8_t *u8Data, StHartDataFrame stHart);

#endif


