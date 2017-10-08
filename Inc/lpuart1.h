#ifndef __LPUART1_H
#define __LPUART1_H
#include "stm32l0xx_hal.h"
#include "stdio.h"

#define LPUART1_RX_BUF_MAX    50
#define RXBUFFERSIZE1	      1	//�����С

extern UART_HandleTypeDef hlpuart1;

extern uint8_t LPUART1_RX_BUF[LPUART1_RX_BUF_MAX];//���ջ�����
extern uint8_t LPUART1_RX_STA;                    //����״̬���
extern uint8_t LPUART1_RX_LEN;                    //�������ݳ���
extern uint8_t aRxBuffer1[RXBUFFERSIZE1];	      //HAL��ʹ�õĴ��ڽ��ջ���

void LPUART1_UART_Init(uint32_t baudrate);
void LPUART1_RX_Reset(void);
void LPUART1_RX_Data_Handle(void);

#endif
