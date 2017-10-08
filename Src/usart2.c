#include "usart2.h"
#include "string.h"
#include "stdarg.h"

UART_HandleTypeDef huart2;

uint8_t USART2_TX_BUF[USART2_TX_BUF_MAX];//���ջ�����
uint8_t USART2_RX_BUF[USART2_RX_BUF_MAX];//���ջ�����
uint8_t USART2_RX_STA = 0;               //����״̬���
uint8_t USART2_RX_LEN = 0;               //�������ݳ���
uint8_t aRxBuffer2[RXBUFFERSIZE2];	     //HAL��ʹ�õĴ��ڽ��ջ���

#if 1//�������´�����֧�ִ���2��u2_printf����
//ȷ��һ�η������ݲ���USART2_TX_BUF_MAX�ֽ�
void u2_printf(char* fmt,...)
{
	uint16_t i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART2_TX_BUF);		//�˴η������ݵĳ���
	for(j=0;j<i;j++)							//ѭ����������
	{
		while((USART2->ISR&0X40)==0);//ѭ������,ֱ���������   
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

	HAL_UART_Receive_IT(&huart2,   (uint8_t *)aRxBuffer2, RXBUFFERSIZE2);//�ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������
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

	if((USART2_RX_STA&0x80)==0x00)//����δ���
	{
		if(USART2_RX_STA&0x40)//ǰһ�����յ������ݽ������ġ�ǰһ�롱
		{
			if     (aRxBuffer2[0]==0x0A)USART2_RX_STA |= 0x80;//���������
			else if(aRxBuffer2[0]!=0x0D)USART2_RX_STA &= 0xBF;//���ǽ���������ǰ�����������־��0
		}
		else
		{
			if(aRxBuffer2[0]==0x0D)USART2_RX_STA |= 0x40;
		}
	}

	if((USART2_RX_STA&0x80)==0x80)//����������һ�����if else��䣨����ֻ������һ���յ����ݵ�ʱ��Ż�����жϣ����ν��յò�����ʱ��Ӧ������λ���ж�USART1_RX_STA����Ҫ�÷�֧��
	{
		USART2_RX_LEN = (USART2_RX_STA&0x3F)-2;//�˴�ע����������ȼ���Ҫ��( )

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
	while (HAL_UART_GetState(&huart2)!=HAL_UART_STATE_READY)//�ȴ�����
	{
		timeout++;////��ʱ����
		if(timeout>maxDelay) break;
	}

	timeout=0;
	while(HAL_UART_Receive_IT(&huart2,(uint8_t *)aRxBuffer2, RXBUFFERSIZE2)!=HAL_OK)//һ�δ������֮�����¿����жϲ�����RxXferCountΪ1
	{
		timeout++; //��ʱ����
		if(timeout>maxDelay) break;
	}
}
