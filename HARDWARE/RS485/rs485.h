#ifndef __RS485_H
#define __RS485_H
#include "sys.h"

//************************RS485��������*************************  


extern u8 RS485_RX_BUF[64]; 		//���ջ���,���64���ֽ�
extern u8 RS485_RX_CNT;   			//���յ������ݳ���

#define RECVBUFF_SIZE 64
#define SENDBUFF_SIZE 60

//����봮���жϽ��գ�����EN_USART2_RXΪ1����������Ϊ0
#define EN_USART1_RX 	1		//0,������;1,����.

void RS485_Init(u32 bound);
void RS485_Send_Data(u8 *buf,u8 len);
void RS485_Receive_Data(u8 *buf,u8 *len);	
void RS485_TX_Set(u8 en);
#endif
