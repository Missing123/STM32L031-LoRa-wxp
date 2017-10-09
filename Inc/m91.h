#ifndef __M91_H
#define __M91_H
#include "stm32l0xx_hal.h"

#define AT_RETRY_TIMES	3

#define AT_TEST			"AT"
#define AT_ECHO_ON		"AT+SYSTEM=1"
#define AT_ECHO_OFF		"AT+SYSTEM=0"
#define AT_OTAA			"AT+OTAA=1"
#define AT_APPEUI		"AT+APPEUI=2C 26 C5 02 AE 00 00 01"
#define AT_APPKEY		"AT+APPKEY=00 11 22 33 44 55 66 77 88 99 AA BB CC DD EE FF"
#define AT_SAVE			"AT+SAVE"
#define AT_RESET		"AT+RESET"

#define ECHO_STR		"OK"
typedef struct {
  uint8_t	Join;
	uint8_t ONline;
	uint8_t BUSY;
	uint8_t NET_Mode;
} Node_Info;
#define LORANODE_STAT_STATUS	HAL_GPIO_ReadPin(LORANODE_STAT_GPIO_PORT, LORANODE_STAT_PIN)
#define LORANODE_BUSY_STATUS	HAL_GPIO_ReadPin(LORANODE_BUSY_GPIO_PORT, LORANODE_BUSY_PIN)
#define LORANODE_MODE_STATUS	HAL_GPIO_ReadPin(LORANODE_MODE_GPIO_PORT, LORANODE_MODE_PIN)
uint8_t M91_Init(void);

#endif
