#ifndef LWIP_CLIENT_APP_H
#define LWIP_CLIENT_APP_H
#include "sys.h"   
 
 
#define TCP_CLIENT_RX_BUFSIZE	1500	//接收缓冲区长度
#define TCP_CLIENT_TX_BUFSIZE	200		//发送缓冲区长度
#define REMOTE_PORT				502		//定义远端从机的端口号
#define LWIP_SEND_DATA			0X80    //定义有数据发送
#define LWIP_DATA_RX_OK		1

extern u8 tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];	//TCP客户端接收数据缓冲区
extern u8 tcp_client_sendbuf[TCP_CLIENT_TX_BUFSIZE];	//TCP客户端发送数据缓冲区
extern u8 tcp_client_flag;		    //TCP客户端数据发送标志位

uint8_t tcp_client_init(void);  //tcp客户端初始化(创建tcp客户端线程)
#endif

