#ifndef __TCP_SERVER_APP_H
#define __TCP_SERVER_APP_H
#include "sys.h"


 
#define TCP_SERVER_RX_BUFSIZE	1000		//定义tcp server最大接收数据长度
#define TCP_SERVER_PORT			8088	//定义tcp server的端口
#define LWIP_SEND_DATA			0X80	//定义有数据发送

extern u8 tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	//TCP服务器接收数据缓冲区
extern u8 tcp_server_flag;			//TCP服务器数据发送标志位

uint8_t tcp_server_init(void);		//TCP服务器初始化(创建TCP服务器线程)

#endif

