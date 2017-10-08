#include "usart2.h"
#include "string.h"
#include "stdarg.h"

UART_HandleTypeDef huart2;

uint8_t USART2_TX_BUF[USART2_TX_BUF_MAX];//接收缓存区
uint8_t USART2_RX_BUF[USART2_RX_BUF_MAX];//接收缓存区
uint8_t USART2_RX_STA = 0;               //接收状态标记
uint8_t USART2_RX_LEN = 0;               //接收数据长度
uint8_t aRxBuffer2[RXBUFFERSIZE2];	     //HAL库使用的串口接收缓冲

#if 1//加入如下代码以支持串口2的u2_printf函数
//确保一次发送数据不超USART2_TX_BUF_MAX字节
void u2_printf(char* fmt,...)
{
	uint16_t i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART2_TX_BUF);		//此次发送数据的长度
	for(j=0;j<i;j++)							//循环发送数据
	{
		while((USART2->ISR&0X40)==0);//循环发送,直到发送完毕   
		USART2->TDR = (uint8_t)USART2_TX_BUF[j];
	}
}
#endif
void USART2_UART_Init(uint32_t baudrate)
{
	huart2.Instance						= USART2;
	huart2.Init.BaudRate				= baudrate;
	huart2.Init.WordLength				= UART_WORDLENGTH_8B;
	huart2.Init.StopBits				= UART_STOPBITS_1;
	huart2.Init.Parity					= UART_PARITY_NONE;
	huart2.Init.Mode					= UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl				= UART_HWCONTROL_NONE;
	huart2.Init.OverSampling			= UART_OVERSAMPLING_16;
	huart2.Init.OneBitSampling			= UART_ONE_BIT_SAMPLE_DISABLE;
	huart2.AdvancedInit.AdvFeatureInit	= UART_ADVFEATURE_NO_INIT;
	HAL_UART_Init(&huart2);

	HAL_UART_Receive_IT(&huart2,   (uint8_t *)aRxBuffer2, RXBUFFERSIZE2);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
}
void USART2_RX_Reset(void)
{
	memset(USART2_RX_BUF,0,USART2_RX_BUF_MAX);
	USART2_RX_STA = 0;
}
void USART2_RX_Data_Handle(void)
{
	USART2_RX_BUF[USART2_RX_STA&0x3F] = aRxBuffer2[0] ;
	USART2_RX_STA++;

	if((USART2_RX_STA&0x80)==0x00)//接收未完成
	{
		if(USART2_RX_STA&0x40)//前一个接收到的数据结束符的“前一半”
		{
			if     (aRxBuffer2[0]==0x0A)USART2_RX_STA |= 0x80;//接收完成了
			else if(aRxBuffer2[0]!=0x0D)USART2_RX_STA &= 0xBF;//不是结束符，则将前半个结束符标志置0
		}
		else
		{
			if(aRxBuffer2[0]==0x0D)USART2_RX_STA |= 0x40;
		}
	}

	if((USART2_RX_STA&0x80)==0x80)//不可与上面一起组成if else语句（否则只有在下一次收到数据的时候才会进入中断，本次接收得不到及时响应，其他位置判断USART1_RX_STA则不需要该分支）
	{
		USART2_RX_LEN = (USART2_RX_STA&0x3F)-2;//此处注意运算符优先级，要加( )

		/************************************/
		USART2_RX_BUF[USART2_RX_LEN] = 0;
		u2_printf("rcv:%s\r\n",USART2_RX_BUF);
		/************************************/

		USART2_RX_Reset();
	}
}
void USART2_IRQHandler(void)
{
	uint32_t timeout=0;
	uint32_t maxDelay=0x1FFFF;

	HAL_UART_IRQHandler(&huart2);

	timeout=0;
	while (HAL_UART_GetState(&huart2)!=HAL_UART_STATE_READY)//等待就绪
	{
		timeout++;////超时处理
		if(timeout>maxDelay) break;
	}

	timeout=0;
	while(HAL_UART_Receive_IT(&huart2,(uint8_t *)aRxBuffer2, RXBUFFERSIZE2)!=HAL_OK)//一次处理完成之后，重新开启中断并设置RxXferCount为1
	{
		timeout++; //超时处理
		if(timeout>maxDelay) break;
	}
}
