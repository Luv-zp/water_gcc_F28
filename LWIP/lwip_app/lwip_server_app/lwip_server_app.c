#include "lwip_server_app.h"
#include "lwip/opt.h"
#include "lwip_comm.h"
#include "lwip/lwip_sys.h"
#include "lwip/api.h" 
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include "delay.h"
#include "universalType.h"
#include "userApp.h"
#include "BLL_Modbus_Server.h"
#include "mb.h"

//TCP服务器任务句柄
TaskHandle_t TCPSERVERTask_Handler;



u8 tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	//TCP服务器接收数据缓冲区


uint8_t tcp_server_thread(void *arg)
{
//	int i = 0;
	BaseType_t res;
	AppMessage appMsg;
	ModbusDev *pModbus = ModbusDev_create();
	u32 data_len = 0;
	struct pbuf *q;
	err_t err,recv_err;
	u8 remot_addr[4];
	struct netconn *conn, *newconn;
	static ip_addr_t ipaddr;
	static u16_t port;
	
	LWIP_UNUSED_ARG(arg);

	conn=netconn_new(NETCONN_TCP);  //创建一个TCP链接
	netconn_bind(conn,IP_ADDR_ANY,TCP_SERVER_PORT);  //绑定端口 8088号端口
	netconn_listen(conn);  		//进入监听模式
	conn->recv_timeout = 10;  	//禁止阻塞线程 等待10ms
	
	while (1) 
	{
		err = netconn_accept(conn,&newconn);  //接收连接请求
		if(err==ERR_OK)newconn->recv_timeout = 10;

		if (err == ERR_OK)    //处理新连接的数据
		{ 
			struct netbuf *recvbuf;

			netconn_getaddr(newconn,&ipaddr,&port,0); //获取远端IP地址和端口号
			
			remot_addr[3] = (uint8_t)(ipaddr.addr >> 24); 
			remot_addr[2] = (uint8_t)(ipaddr.addr>> 16);
			remot_addr[1] = (uint8_t)(ipaddr.addr >> 8);
			remot_addr[0] = (uint8_t)(ipaddr.addr);
			printf("主机%d.%d.%d.%d连接上服务器,主机端口号为:%d\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3],port);
			
			while(1)
			{
				//通过网络发送数据
				res = xQueueReceive(server_netQ, &appMsg, 0);	//获取消息队列中的数据
				if(res == pdTRUE)
				{
					err = netconn_write(newconn, (u8 *)appMsg.pVoid, appMsg.dataType, NETCONN_COPY);
					if(err != ERR_OK)
					{
						printf("服务端发送失败\r\n");
					}
				}
				//接收到数据
				if((recv_err = netconn_recv(newconn, &recvbuf)) == ERR_OK)
				{
					taskENTER_CRITICAL();
					memset(tcp_server_recvbuf, 0, TCP_SERVER_RX_BUFSIZE);	//数据接收缓冲区清零
					for(q=recvbuf->p;q!=NULL;q=q->next)  //遍历完整个pbuf链表
					{
						//判断要拷贝到TCP_SERVER_RX_BUFSIZE中的数据是否大于TCP_SERVER_RX_BUFSIZE的剩余空间，如果大于
						//的话就只拷贝TCP_SERVER_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
						if(q->len > (TCP_SERVER_RX_BUFSIZE-data_len)) 
							memcpy(tcp_server_recvbuf+data_len,q->payload,(TCP_SERVER_RX_BUFSIZE-data_len));//拷贝数据
						else memcpy(tcp_server_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;  	
						if(data_len > TCP_SERVER_RX_BUFSIZE) break; //超出TCP客户端接收数组,跳出	
					}		
					taskEXIT_CRITICAL();	//开中断	
					
//					for(; i < data_len; i ++)
//					{
//						printf("%d\r\n",tcp_server_recvbuf[i]);
//					}
					data_len = 0;		//复制完成后data_len要清零
					
					pModbus->pRecvBuff = tcp_server_recvbuf;
					eMBTcpPoll();
					
					netbuf_delete(recvbuf);		
				}
				else if(recv_err == ERR_CLSD)  //关闭连接
				{
					netconn_close(newconn);
					netconn_delete(newconn);
					printf("客户端%d.%d.%d.%d断开连接\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3]);
					break;
				}
			}
		}
	}
}

//创建TCP服务器线程
//返回值:0 TCP服务器创建成功
//		其他 TCP服务器创建失败
uint8_t tcp_server_init(void)
{
	BaseType_t res;
	
	taskENTER_CRITICAL();              
	res = xTaskCreate((TaskFunction_t)tcp_server_thread,
					(const char*  )"tcp_server_task",
					(uint16_t     )TCPSERVER_STK_SIZE,
					(void*        )NULL,
					(UBaseType_t  )TCPSERVER_TASK_PRIO,
					(TaskHandle_t*)&TCPSERVERTask_Handler);		
	taskEXIT_CRITICAL();
	
	if(res == pdPASS)
	{
		return 0;
	}
	return 1;
}

