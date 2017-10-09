#include "lpuart1.h"
#include "string.h"
#include "usart2.h"
UART_HandleTypeDef hlpuart1;

uint8_t LPUART1_RX_BUF[LPUART1_RX_BUF_MAX];//���ջ�����
uint8_t LPUART1_RX_STA = 0;                //����״̬���
uint8_t LPUART1_RX_LEN = 0;                //�������ݳ���
uint8_t aRxBuffer1[RXBUFFERSIZE1];	       //HAL��ʹ�õĴ��ڽ��ջ���

#if 1//�������´�����֧�ִ���1��printf����
#pragma import(__use_no_semihosting)
//��׼����Ҫ��֧�ֺ���
struct __FILE 
{
	int handle;
};
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{
	x = x; 
}
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{
	while((LPUART1->ISR&0X40)==0){}//ѭ������,ֱ���������   
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

	HAL_UART_Receive_IT(&hlpuart1, (uint8_t *)aRxBuffer1, RXBUFFERSIZE1);//�ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������
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

	if((LPUART1_RX_STA&0x80)==0x00)//����δ���
	{
		if(LPUART1_RX_STA&0x40)//ǰһ�����յ������ݽ������ġ�ǰһ�롱
		{
			if     (aRxBuffer1[0]==0x0A)LPUART1_RX_STA |= 0x80;//���������
			else if(aRxBuffer1[0]!=0x0D)LPUART1_RX_STA &= 0xBF;//���ǽ���������ǰ�����������־��0
		}
		else
		{
			if(aRxBuffer1[0]==0x0D)LPUART1_RX_STA |= 0x40;
		}
	}

	if((LPUART1_RX_STA&0x80)==0x80)//����������һ�����if else��䣨����ֻ������һ���յ����ݵ�ʱ��Ż�����жϣ����ν��յò�����ʱ��Ӧ������λ���ж�USART1_RX_STA����Ҫ�÷�֧��
	{
		LPUART1_RX_LEN = (LPUART1_RX_STA&0x3F)-2;//�˴�ע����������ȼ���Ҫ��( )

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
	while (HAL_UART_GetState(&hlpuart1)!=HAL_UART_STATE_READY)//�ȴ�����
	{
		timeout++;////��ʱ����
		if(timeout>maxDelay) break;
	}

	timeout=0;
	while(HAL_UART_Receive_IT(&hlpuart1,(uint8_t *)aRxBuffer1, RXBUFFERSIZE1)!=HAL_OK)//һ�δ������֮�����¿����жϲ�����RxXferCountΪ1
	{
		timeout++; //��ʱ����
		if(timeout>maxDelay) break;
	}
}
