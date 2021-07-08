#include "rs485.h"
#include "pcf8574.h"
#include "delay.h"
#include "led.h"
#include "FreeRTOS.H"
#include "task.h"
//***************************RS485��������*************************	   


UART_HandleTypeDef USART1_RS485Handler;  //USART2���(����RS485)

#if EN_USART1_RX   		//���ʹ���˽���   	  
//���ջ����� 	
u8 RS485_RX_BUF[64];  	//���ջ���,���64���ֽ�.
//���յ������ݳ���
u8 RS485_RX_CNT=0;  

void USART1_IRQHandler(void)
{
    u8 res;
//	if(__HAL_UART_GET_IT(&USART1_RS485Handler, UART_IT_ORE)!=RESET)
//	{
//		__HAL_UART_CLEAR_OREFLAG(&USART1_RS485Handler);
//	}
	
    if(__HAL_UART_GET_IT(&USART1_RS485Handler,UART_IT_RXNE)!=RESET)  //�����ж�
	{	 	
        HAL_UART_Receive(&USART1_RS485Handler,&res,1,1000);
//		LED1(0);
		if(RS485_RX_CNT<64)
		{
			RS485_RX_BUF[RS485_RX_CNT]=res;		//��¼���յ���ֵ
			RS485_RX_CNT++;						//������������1 
		}
		else
		{
			RS485_RX_CNT = 0;
		}
	}

//	if(__HAL_UART_GET_FLAG(&USART1_RS485Handler, UART_FLAG_NE)!=RESET)
//	{
//		__HAL_UART_CLEAR_NEFLAG(&USART1_RS485Handler);
//	}
//	
//	if(__HAL_UART_GET_FLAG(&USART1_RS485Handler, UART_FLAG_FE)!=RESET)
//	{
//		__HAL_UART_CLEAR_FEFLAG(&USART1_RS485Handler);
//	}
//	
//	if(__HAL_UART_GET_FLAG(&USART1_RS485Handler, UART_FLAG_PE)!=RESET)
//	{
//		__HAL_UART_CLEAR_PEFLAG(&USART1_RS485Handler);
//	}
}    
#endif

//��ʼ��IO ����2
//bound:������
void RS485_Init(u32 bound)
{
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_Initure;
	
    PCF8574_Init();                         //��ʼ��PCF8574�����ڿ���RE��
	
	__HAL_RCC_GPIOA_CLK_ENABLE();			//ʹ��GPIOAʱ��
	__HAL_RCC_USART1_CLK_ENABLE();			//ʹ��USART1ʱ��

	GPIO_Initure.Pin=GPIO_PIN_9|GPIO_PIN_10;			//PA9,PA10
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
	GPIO_Initure.Pull=GPIO_PULLUP;			//����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;		//����
	GPIO_Initure.Alternate=GPIO_AF7_USART1;	//����ΪUSART1
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PA9

    
    //USART ��ʼ������
	USART1_RS485Handler.Instance=USART1;			        //USART1
	USART1_RS485Handler.Init.BaudRate=bound;		        //������
	USART1_RS485Handler.Init.WordLength=UART_WORDLENGTH_8B;	//�ֳ�Ϊ8λ���ݸ�ʽ
	USART1_RS485Handler.Init.StopBits=UART_STOPBITS_1;		//һ��ֹͣλ
	USART1_RS485Handler.Init.Parity=UART_PARITY_NONE;		//У��λ
	USART1_RS485Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;	//��Ӳ������
	USART1_RS485Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&USART1_RS485Handler);			        //HAL_UART_Init()��ʹ��USART1
	
    
    __HAL_UART_CLEAR_IT(&USART1_RS485Handler,UART_CLEAR_TCF);
#if EN_USART1_RX
	__HAL_UART_ENABLE_IT(&USART1_RS485Handler,UART_IT_RXNE);//���������ж�
	
//	__HAL_UART_ENABLE_IT(&USART1_RS485Handler, UART_IT_ERR);
//	__HAL_UART_ENABLE_IT(&USART1_RS485Handler, UART_IT_PE);
	HAL_NVIC_EnableIRQ(USART1_IRQn);				        //ʹ��USART1�ж�
	HAL_NVIC_SetPriority(USART1_IRQn,9,0);			        //��ռ���ȼ�3�������ȼ�3
#endif	
    RS485_TX_Set(0);                                        //����Ϊ����ģʽ	
}

//RS485����len���ֽ�.
//buf:�������׵�ַ
//len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
void RS485_Send_Data(u8 *buf,u8 len)
{
	RS485_TX_Set(1);			//����Ϊ����ģʽ
    HAL_UART_Transmit(&USART1_RS485Handler,buf,len,1000);//����2��������
	RS485_RX_CNT=0;	  
	RS485_TX_Set(0);			//����Ϊ����ģʽ	
}

//RS485��ѯ���յ�������
//buf:���ջ����׵�ַ
//len:���������ݳ���
void RS485_Receive_Data(u8 *buf,u8 *len)
{
	u8 rxlen = RS485_RX_CNT;
	u8 i=0;
	*len=0;				//Ĭ��Ϊ0
	delay_xms(10);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
	if(rxlen == RS485_RX_CNT && rxlen)//���յ�������,�ҽ��������
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i] = RS485_RX_BUF[i];	
		}		
		*len = RS485_RX_CNT;	//��¼�������ݳ���
		RS485_RX_CNT = 0;		//����
	}
} 

//RS485ģʽ����.
//en:0,����;1,����.
void RS485_TX_Set(u8 en)
{
	PCF8574_WriteBit(RS485_RE_IO,en);
}
