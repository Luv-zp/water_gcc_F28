#ifndef _RS232_H
#define _RS232_H
#include "sys.h"


#define RECEVBUFF_SIZE 64
#define SENDBUFF_SIZE 64

#define EN_USART3_RX 1


extern u8 rs232_RxBuf[RECEVBUFF_SIZE];
extern u8 rs232_TxBuf[SENDBUFF_SIZE];
extern u8 rs232_Rx_CNT;

void RS232_init(u32 bound);
void rs232_send_data(u8 *pBuf, u8 len);
void RS232_Receive_Data(u8 *buf,u8 *len);

#endif

