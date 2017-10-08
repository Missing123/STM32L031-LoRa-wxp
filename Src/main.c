#include "lpuart1.h"
#include "usart2.h"

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART2)
	{
		USART2_RX_Data_Handle();
	}
	if(huart->Instance==LPUART1)
	{
		LPUART1_RX_Data_Handle();
	}
}
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_LPUART1;
	PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
	PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}
int main(void)
{
	HAL_Init();
	SystemClock_Config();

	USART2_UART_Init(115200);
	HAL_UART_Transmit(&huart2,  "usart2 ok\r\n", sizeof("usart2 ok\r\n")-1, 1000);

	LPUART1_UART_Init(115200);
	HAL_UART_Transmit(&hlpuart1,"lpuart1 ok\r\n",sizeof("lpuart1 ok\r\n")-1,1000);

	while(1)
	{
		HAL_Delay(500);
	}
}
