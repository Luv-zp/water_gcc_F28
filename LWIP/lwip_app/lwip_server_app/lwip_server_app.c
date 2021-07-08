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

//TCP������������
TaskHandle_t TCPSERVERTask_Handler;



u8 tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	//TCP�������������ݻ�����


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

	conn=netconn_new(NETCONN_TCP);  //����һ��TCP����
	netconn_bind(conn,IP_ADDR_ANY,TCP_SERVER_PORT);  //�󶨶˿� 8088�Ŷ˿�
	netconn_listen(conn);  		//�������ģʽ
	conn->recv_timeout = 10;  	//��ֹ�����߳� �ȴ�10ms
	
	while (1) 
	{
		err = netconn_accept(conn,&newconn);  //������������
		if(err==ERR_OK)newconn->recv_timeout = 10;

		if (err == ERR_OK)    //���������ӵ�����
		{ 
			struct netbuf *recvbuf;

			netconn_getaddr(newconn,&ipaddr,&port,0); //��ȡԶ��IP��ַ�Ͷ˿ں�
			
			remot_addr[3] = (uint8_t)(ipaddr.addr >> 24); 
			remot_addr[2] = (uint8_t)(ipaddr.addr>> 16);
			remot_addr[1] = (uint8_t)(ipaddr.addr >> 8);
			remot_addr[0] = (uint8_t)(ipaddr.addr);
			printf("����%d.%d.%d.%d�����Ϸ�����,�����˿ں�Ϊ:%d\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3],port);
			
			while(1)
			{
				//ͨ�����緢������
				res = xQueueReceive(server_netQ, &appMsg, 0);	//��ȡ��Ϣ�����е�����
				if(res == pdTRUE)
				{
					err = netconn_write(newconn, (u8 *)appMsg.pVoid, appMsg.dataType, NETCONN_COPY);
					if(err != ERR_OK)
					{
						printf("����˷���ʧ��\r\n");
					}
				}
				//���յ�����
				if((recv_err = netconn_recv(newconn, &recvbuf)) == ERR_OK)
				{
					taskENTER_CRITICAL();
					memset(tcp_server_recvbuf, 0, TCP_SERVER_RX_BUFSIZE);	//���ݽ��ջ���������
					for(q=recvbuf->p;q!=NULL;q=q->next)  //����������pbuf����
					{
						//�ж�Ҫ������TCP_SERVER_RX_BUFSIZE�е������Ƿ����TCP_SERVER_RX_BUFSIZE��ʣ��ռ䣬�������
						//�Ļ���ֻ����TCP_SERVER_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
						if(q->len > (TCP_SERVER_RX_BUFSIZE-data_len)) 
							memcpy(tcp_server_recvbuf+data_len,q->payload,(TCP_SERVER_RX_BUFSIZE-data_len));//��������
						else memcpy(tcp_server_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;  	
						if(data_len > TCP_SERVER_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
					}		
					taskEXIT_CRITICAL();	//���ж�	
					
//					for(; i < data_len; i ++)
//					{
//						printf("%d\r\n",tcp_server_recvbuf[i]);
//					}
					data_len = 0;		//������ɺ�data_lenҪ����
					
					pModbus->pRecvBuff = tcp_server_recvbuf;
					eMBTcpPoll();
					
					netbuf_delete(recvbuf);		
				}
				else if(recv_err == ERR_CLSD)  //�ر�����
				{
					netconn_close(newconn);
					netconn_delete(newconn);
					printf("�ͻ���%d.%d.%d.%d�Ͽ�����\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3]);
					break;
				}
			}
		}
	}
}

//����TCP�������߳�
//����ֵ:0 TCP�����������ɹ�
//		���� TCP����������ʧ��
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

