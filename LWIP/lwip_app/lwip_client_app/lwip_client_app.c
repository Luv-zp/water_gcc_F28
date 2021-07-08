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

//TCP�ͻ���������
TaskHandle_t TCPCLIENTTask_Handler;


struct netconn *tcp_clientconn = NULL;					//TCP CLIENT�������ӽṹ��
u8 tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];	//TCP�ͻ��˽������ݻ�����
u8 tcp_client_sendbuf[TCP_CLIENT_TX_BUFSIZE];	//TCP�ͻ��˷������ݻ�����

//tcp�ͻ���������
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
		tcp_clientconn=netconn_new(NETCONN_TCP);  //����һ��TCP����
        err = netconn_connect(tcp_clientconn,&server_ipaddr,server_port);//���ӷ�����
		if(err != ERR_OK)  
			netconn_delete(tcp_clientconn); //����ֵ������ERR_OK,ɾ��tcp_clientconn����
		else if (err == ERR_OK)    //���������ӵ�����
		{ 
			struct netbuf *recvbuf;
			tcp_clientconn->recv_timeout = 10;			//10ms ���ղ������� ��recv_err == ERR_CLSD
			netconn_getaddr(tcp_clientconn,&loca_ipaddr,&loca_port,1); //��ȡ����IP����IP��ַ�Ͷ˿ں�
			printf("�����Ϸ�����%d.%d.%d.%d,�����˿ں�Ϊ:%d\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3],loca_port);
			while(1)
			{
				res = xQueueReceive(client_netQ, &appMsg, portMAX_DELAY); //����
				if(res == pdTRUE)
				{
//					for(i = 0; i < appMsg.dataType; i++)
//					{
//						printf("%d\r\n",((u8 *)appMsg.pVoid)[i]);
//					}
					err = netconn_write(tcp_clientconn,(u8 *)appMsg.pVoid, appMsg.dataType, NETCONN_COPY); //����tcp_client_sendbuf�е�����
					if(err != ERR_OK)
					printf("����ʧ��\r\n");
				}
				if((recv_err = netconn_recv(tcp_clientconn,&recvbuf)) == ERR_OK)  //���յ�����
				{
					taskENTER_CRITICAL();	//���ж�		
					memset(tcp_client_recvbuf,0,TCP_CLIENT_RX_BUFSIZE);  //���ݽ��ջ���������
					for(q=recvbuf->p;q!=NULL;q=q->next)  //����������pbuf����
					{
						//�ж�Ҫ������TCP_CLIENT_RX_BUFSIZE�е������Ƿ����TCP_CLIENT_RX_BUFSIZE��ʣ��ռ䣬�������
						//�Ļ���ֻ����TCP_CLIENT_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
						if(q->len > (TCP_CLIENT_RX_BUFSIZE-data_len)) 
							memcpy(tcp_client_recvbuf+data_len,q->payload,(TCP_CLIENT_RX_BUFSIZE-data_len));//��������
						else 
							memcpy(tcp_client_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;  	
						if(data_len > TCP_CLIENT_RX_BUFSIZE) 
							break; //����TCP�ͻ��˽�������,����	
					}		
					taskEXIT_CRITICAL();	//���ж�	
					
//					for(i = 0; i < data_len; i++)
//					{
//						printf("%d\r\n",tcp_client_recvbuf[i]);
//					}
					data_len=0;  //������ɺ�data_lenҪ���㡣					
					//���ݴ���		
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
				else if(recv_err == ERR_CLSD)  //�ر�����
				{
					netconn_close(tcp_clientconn);
					netconn_delete(tcp_clientconn);
					printf("������%d.%d.%d.%d�Ͽ�����\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3]);
					break;
				}
			}
		}
	}
}


//����TCP�ͻ����߳�
//����ֵ:0 TCP�ͻ��˴����ɹ�
//		���� TCP�ͻ��˴���ʧ��
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
