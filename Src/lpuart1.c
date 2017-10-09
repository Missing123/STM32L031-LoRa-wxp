#include "lpuart1.h"
#include "string.h"
#include "usart2.h"
UART_HandleTypeDef hlpuart1;

uint8_t LPUART1_RX_BUF[LPUART1_RX_BUF_MAX];//接收缓存区
uint8_t LPUART1_RX_STA = 0;                //接收状态标记
uint8_t LPUART1_RX_LEN = 0;                //接收数据长度
uint8_t aRxBuffer1[RXBUFFERSIZE1];	       //HAL库使用的串口接收缓冲

#if 1//加入如下代码以支持串口1的printf函数
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE 
{
	int handle;
};
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{
	x = x; 
}
//重定义fputc函数 
int fputc(int ch, FILE *f)
{
	while((LPUART1->ISR&0X40)==0){}//循环发送,直到发送完毕   
    LPUART1->TDR = (uint8_t) ch;      
	return ch;
}
#endif
void LPUART1_UART_Init(uint32_t baudrate)
{
	hlpuart1.Instance						= LPUART1;
	hlpuart1.Init.BaudRate					= baudrate;
	hlpuart1.Init.WordLength				= UART_WORDLENGTH_8B;
	hlpuart1.Init.StopBits					= UART_STOPBITS_1;
	hlpuart1.Init.Parity					= UART_PARITY_NONE;
	hlpuart1.Init.Mode						= UART_MODE_TX_RX;
	hlpuart1.Init.HwFlowCtl					= UART_HWCONTROL_NONE;
	hlpuart1.Init.OneBitSampling			= UART_ONE_BIT_SAMPLE_DISABLE;
	hlpuart1.AdvancedInit.AdvFeatureInit	= UART_ADVFEATURE_NO_INIT;
	HAL_UART_Init(&hlpuart1);

	HAL_UART_Receive_IT(&hlpuart1, (uint8_t *)aRxBuffer1, RXBUFFERSIZE1);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
}
void LPUART1_RX_Reset(void)
{
	memset(LPUART1_RX_BUF,0,LPUART1_RX_BUF_MAX);
	LPUART1_RX_STA = 0;
}
void LPUART1_RX_Data_Handle(void)
{
	LPUART1_RX_BUF[LPUART1_RX_STA&0x3F] = aRxBuffer1[0] ;
	LPUART1_RX_STA++;

	if((LPUART1_RX_STA&0x80)==0x00)//接收未完成
	{
		if(LPUART1_RX_STA&0x40)//前一个接收到的数据结束符的“前一半”
		{
			if     (aRxBuffer1[0]==0x0A)LPUART1_RX_STA |= 0x80;//接收完成了
			else if(aRxBuffer1[0]!=0x0D)LPUART1_RX_STA &= 0xBF;//不是结束符，则将前半个结束符标志置0
		}
		else
		{
			if(aRxBuffer1[0]==0x0D)LPUART1_RX_STA |= 0x40;
		}
	}

	if((LPUART1_RX_STA&0x80)==0x80)//不可与上面一起组成if else语句（否则只有在下一次收到数据的时候才会进入中断，本次接收得不到及时响应，其他位置判断USART1_RX_STA则不需要该分支）
	{
		LPUART1_RX_LEN = (LPUART1_RX_STA&0x3F)-2;//此处注意运算符优先级，要加( )

		/************************************/
		LPUART1_RX_BUF[LPUART1_RX_LEN] = 0;
		printf("rcv:%s\r\n",USART2_RX_BUF);
		/************************************/

		LPUART1_RX_Reset();
	}
}
void LPUART1_IRQHandler(void)
{
	uint32_t timeout=0;
	uint32_t maxDelay=0x1FFFF;

	HAL_UART_IRQHandler(&hlpuart1);
	
	timeout=0;
	while (HAL_UART_GetState(&hlpuart1)!=HAL_UART_STATE_READY)//等待就绪
	{
		timeout++;////超时处理
		if(timeout>maxDelay) break;
	}

	timeout=0;
	while(HAL_UART_Receive_IT(&hlpuart1,(uint8_t *)aRxBuffer1, RXBUFFERSIZE1)!=HAL_OK)//一次处理完成之后，重新开启中断并设置RxXferCount为1
	{
		timeout++; //超时处理
		if(timeout>maxDelay) break;
	}
}
