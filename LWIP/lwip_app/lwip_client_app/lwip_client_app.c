#include "lwip_client_app.h"
#include "lwip/opt.h"
#include "lwip_comm.h"
#include "lwip/lwip_sys.h"
#include "lwip/api.h" 
#include "userApp.h"
#include "universalType.h"
#include <string.h>
#include "delay.h"
#include "F28_MbTcpProtocol.h"
#include "BLL_SubLeakTest.h"

//TCP客户端任务句柄
TaskHandle_t TCPCLIENTTask_Handler;


struct netconn *tcp_clientconn = NULL;					//TCP CLIENT网络连接结构体
u8 tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];	//TCP客户端接收数据缓冲区
u8 tcp_client_sendbuf[TCP_CLIENT_TX_BUFSIZE];	//TCP客户端发送数据缓冲区

//tcp客户端任务函数
static void tcp_client_thread(void *arg)
{
//	int i = 0;
	BaseType_t res;
	AppMessage appMsg;
	u32 data_len = 0;
	MBTCP_PROTOCOL_FRAME netTcp_RxFrame;
	CF28Light * pCF28Lpv = CF28Light_create();
	struct pbuf *q;
	err_t err,recv_err;
	static ip_addr_t server_ipaddr,loca_ipaddr;
	static u16_t 		 server_port,loca_port;

	LWIP_UNUSED_ARG(arg);
	server_port = REMOTE_PORT;
	IP4_ADDR(&server_ipaddr, lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3]);
	
	while (1) 
	{
		tcp_clientconn=netconn_new(NETCONN_TCP);  //创建一个TCP链接
        err = netconn_connect(tcp_clientconn,&server_ipaddr,server_port);//连接服务器
		if(err != ERR_OK)  
			netconn_delete(tcp_clientconn); //返回值不等于ERR_OK,删除tcp_clientconn连接
		else if (err == ERR_OK)    //处理新连接的数据
		{ 
			struct netbuf *recvbuf;
			tcp_clientconn->recv_timeout = 10;			//10ms 接收不到数据 则recv_err == ERR_CLSD
			netconn_getaddr(tcp_clientconn,&loca_ipaddr,&loca_port,1); //获取本地IP主机IP地址和端口号
			printf("连接上服务器%d.%d.%d.%d,本机端口号为:%d\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3],loca_port);
			while(1)
			{
				res = xQueueReceive(client_netQ, &appMsg, portMAX_DELAY); //死等
				if(res == pdTRUE)
				{
//					for(i = 0; i < appMsg.dataType; i++)
//					{
//						printf("%d\r\n",((u8 *)appMsg.pVoid)[i]);
//					}
					err = netconn_write(tcp_clientconn,(u8 *)appMsg.pVoid, appMsg.dataType, NETCONN_COPY); //发送tcp_client_sendbuf中的数据
					if(err != ERR_OK)
					printf("发送失败\r\n");
				}
				if((recv_err = netconn_recv(tcp_clientconn,&recvbuf)) == ERR_OK)  //接收到数据
				{
					taskENTER_CRITICAL();	//关中断		
					memset(tcp_client_recvbuf,0,TCP_CLIENT_RX_BUFSIZE);  //数据接收缓冲区清零
					for(q=recvbuf->p;q!=NULL;q=q->next)  //遍历完整个pbuf链表
					{
						//判断要拷贝到TCP_CLIENT_RX_BUFSIZE中的数据是否大于TCP_CLIENT_RX_BUFSIZE的剩余空间，如果大于
						//的话就只拷贝TCP_CLIENT_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
						if(q->len > (TCP_CLIENT_RX_BUFSIZE-data_len)) 
							memcpy(tcp_client_recvbuf+data_len,q->payload,(TCP_CLIENT_RX_BUFSIZE-data_len));//拷贝数据
						else 
							memcpy(tcp_client_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;  	
						if(data_len > TCP_CLIENT_RX_BUFSIZE) 
							break; //超出TCP客户端接收数组,跳出	
					}		
					taskEXIT_CRITICAL();	//开中断	
					
//					for(i = 0; i < data_len; i++)
//					{
//						printf("%d\r\n",tcp_client_recvbuf[i]);
//					}
					data_len=0;  //复制完成后data_len要清零。					
					//数据处理		
					ReceDataStructUnpack(tcp_client_recvbuf, &netTcp_RxFrame);
					if(netTcp_RxFrame.u16TID == READ_REALTIME_TID)
					{
						pCF28Lpv->GetRealTimeData(pCF28Lpv, &netTcp_RxFrame);
					}
					else if(netTcp_RxFrame.u16TID == READ_LAST_TID)
					{
						pCF28Lpv->GetLastResult(pCF28Lpv, &netTcp_RxFrame);
					}
					
					netbuf_delete(recvbuf);
									
				}
				else if(recv_err == ERR_CLSD)  //关闭连接
				{
					netconn_close(tcp_clientconn);
					netconn_delete(tcp_clientconn);
					printf("服务器%d.%d.%d.%d断开连接\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3]);
					break;
				}
			}
		}
	}
}


//创建TCP客户端线程
//返回值:0 TCP客户端创建成功
//		其他 TCP客户端创建失败
uint8_t tcp_client_init(void)
{
	BaseType_t res;
	
	taskENTER_CRITICAL();              
	res = xTaskCreate((TaskFunction_t)tcp_client_thread,
					(const char*  )"tcp_client_task",
					(uint16_t     )TCPCLIENT_STK_SIZE,
					(void*        )NULL,
					(UBaseType_t  )TCPCLIENT_TASK_PRIO,
					(TaskHandle_t*)&TCPCLIENTTask_Handler);		
	taskEXIT_CRITICAL();
	
	if(res == pdPASS)
	{
		return 0;
	}
	return 1;
}
