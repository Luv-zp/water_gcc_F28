#ifndef __TCP_SERVER_APP_H
#define __TCP_SERVER_APP_H
#include "sys.h"


 
#define TCP_SERVER_RX_BUFSIZE	1000		//����tcp server���������ݳ���
#define TCP_SERVER_PORT			8088	//����tcp server�Ķ˿�
#define LWIP_SEND_DATA			0X80	//���������ݷ���

extern u8 tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	//TCP�������������ݻ�����
extern u8 tcp_server_flag;			//TCP���������ݷ��ͱ�־λ

uint8_t tcp_server_init(void);		//TCP��������ʼ��(����TCP�������߳�)

#endif

