/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __gpio_H
#define __gpio_H
#ifdef __cplusplus
 extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
#include "main.h"

/* USER CODE BEGIN Includes */
#define LORANODE_WAKE_PIN					GPIO_PIN_0
#define LORANODE_WAKE_GPIO_PORT         	GPIOA
												
#define LORANODE_MODE_PIN					GPIO_PIN_5
#define LORANODE_MODE_GPIO_PORT         	GPIOB
													
#define LORANODE_NRST_PIN					GPIO_PIN_3
#define LORANODE_NRST_GPIO_PORT         	GPIOB
													
/* INPUT */													
#define LORANODE_STAT_PIN					GPIO_PIN_4
#define LORANODE_STAT_GPIO_PORT         	GPIOA
														
#define LORANODE_BUSY_PIN					GPIO_PIN_4
#define LORANODE_BUSY_GPIO_PORT         	GPIOB
/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */
void LPUART1_GPIO_Init(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
