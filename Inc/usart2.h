#ifndef __USART2_H
#define __USART2_H
#include "stm32l0xx_hal.h"

#define USART2_TX_BUF_MAX    200
#define USART2_RX_BUF_MAX    200
#define RXBUFFERSIZE2	     1	//�����С

extern UART_HandleTypeDef huart2;

extern uint8_t USART2_TX_BUF[USART2_TX_BUF_MAX];//���ջ�����
extern uint8_t USART2_RX_BUF[USART2_RX_BUF_MAX];//���ջ�����
extern uint8_t USART2_RX_STA;                   //����״̬���
extern uint8_t USART2_RX_LEN;                   //�������ݳ���
extern uint8_t aRxBuffer2[RXBUFFERSIZE2];	    //HAL��ʹ�õĴ��ڽ��ջ���

void u2_printf(char* fmt,...);
void USART2_UART_Init(uint32_t baudrate);
void USART2_RX_Reset(void);
void USART2_RX_Data_Handle(void);

#endif
