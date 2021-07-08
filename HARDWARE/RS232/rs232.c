#include "rs232.h"
#include "HartComTask.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"

UART_HandleTypeDef USART3_RS232Handler;  //UART3句柄(用于RS232)


u8 rs232_TxBuf[SENDBUFF_SIZE];


#if EN_USART3_RX   		//如果使能了接收   	  
//接收缓存区 	
u8 rs232_RxBuf[RECEVBUFF_SIZE];  	//接收缓冲,最大64个字节.
//接收到的数据长度
u8 rs232_Rx_CNT=0;  

void USART3_IRQHandler(void)
{
    u8 res;	
//	LED0_Toggle;
    if(__HAL_UART_GET_IT(&USART3_RS232Handler,UART_IT_RXNE)!=RESET)  //接收中断
	{	 	
		
        HAL_UART_Receive(&USART3_RS232Handler,&res,1,1000);
		rs232RecvData(res);
////		if(rs232_Rx_CNT < RECEVBUFF_SIZE)
////		{
////			rs232_RxBuf[rs232_Rx_CNT]=res;		//记录接收到的值
////			rs232_Rx_CNT++;						//接收数据增加1 
////		} 
//		if(rs232_Rx_CNT == RECEVBUFF_SIZE)
//		{
//			rs232_Rx_CNT = 0;
//		}
//		rs232_RxBuf[rs232_Rx_CNT++]=res;		//记录接收到的值
		
	} 
}    
#endif


void RS232_init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOB_CLK_ENABLE();			//使能GPIOB时钟
	__HAL_RCC_USART3_CLK_ENABLE();			//使能USART3时钟
	
	GPIO_Initure.Pin=GPIO_PIN_10|GPIO_PIN_11; //PB10,11
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;		//高速
	GPIO_Initure.Alternate=GPIO_AF7_USART3;	//复用为USART3
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);	   	//初始化PB10,11

	/* USART 初始化设置 */
	USART3_RS232Handler.Instance = USART3;
	USART3_RS232Handler.Init.BaudRate = bound;
	USART3_RS232Handler.Init.WordLength = UART_WORDLENGTH_8B;
	USART3_RS232Handler.Init.StopBits = UART_STOPBITS_1;
	USART3_RS232Handler.Init.Parity = UART_PARITY_NONE;
	USART3_RS232Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	USART3_RS232Handler.Init.Mode = UART_MODE_TX_RX;
	HAL_UART_Init(&USART3_RS232Handler);
	
	__HAL_UART_CLEAR_IT(&USART3_RS232Handler, USART_CLEAR_TCF);

#if EN_USART3_RX
	__HAL_UART_ENABLE_IT(&USART3_RS232Handler,UART_IT_RXNE);//开启接收中断
	HAL_NVIC_EnableIRQ(USART3_IRQn);				        //使能USART3中断
	HAL_NVIC_SetPriority(USART3_IRQn,6,0);			        //抢占优先级5，子优先级0
#endif	
}

void rs232_send_data(u8 *pBuf, u8 len)
{
	HAL_UART_Transmit(&USART3_RS232Handler, pBuf, len, 1000);
//	rs232_Rx_CNT = 0;
}

void RS232_Receive_Data(u8 *buf,u8 *len)
{
	u8 rxlen=rs232_Rx_CNT;
	u8 i=0;
	*len=0;				//默认为0
	vTaskDelay(10);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
	if(rxlen==rs232_Rx_CNT&&rxlen)//接收到了数据,且接收完成了
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=rs232_RxBuf[i];	
		}		
		*len=rs232_Rx_CNT;	//记录本次数据长度
		rs232_Rx_CNT=0;		//清零
	}
} 



