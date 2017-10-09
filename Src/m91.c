#include "m91.h"
#include "lpuart1.h"
#include "usart2.h"
#include "gpio.h"
#include "string.h"
uint8_t  comd_mode =1;
Node_Info LoRa_Node;
Node_Info *LoRa_temp = &LoRa_Node;
//waittime必须大于1，单位10ms
//返回值：1-成功，0-失败
uint8_t AT_send_cmd(char *cmd,char *ack,uint16_t waittime)
{
	uint8_t res = 1;//无需应答时默认返回值为1（成功）

	u2_printf("%s\r\n",cmd);//发送命令

	if(ack&&waittime)		//需要等待应答
	{
		while(waittime)	//等待倒计时
		{
			if(strstr((const char*)USART2_RX_BUF,(const char*)ack))
			{
				break;//得到有效数据
			}
			else HAL_Delay(10);
			waittime--;
		}
		if(waittime==0) res = 0;
	}

	HAL_Delay(10);
	USART2_RX_Reset();

	return res;
}

//返回值：8-成功，非0-错误代码
uint8_t M91_Init(void)
{
	uint8_t retry;
	if(comd_mode==1)
	{
			retry = AT_RETRY_TIMES;
			while(retry--)//检测串口是否准备好
			{
				if(AT_send_cmd(AT_TEST,ECHO_STR,200))
				{
					u2_printf("success 1\r\n");
					break;
				}
				else
				{
					u2_printf("failure 1 %01d\r\n",AT_RETRY_TIMES-retry);
				}
				if(retry==0) return 1;
			}

			retry = AT_RETRY_TIMES;
			while(retry--)//开启回显
			{
				if(AT_send_cmd(AT_ECHO_ON,ECHO_STR,200))
				{
					u2_printf("success 2\r\n");
					break;
				}
				else
				{
					u2_printf("failure 2 %01d\r\n",AT_RETRY_TIMES-retry);
				}
				if(retry==0) return 2;
			}

			retry = AT_RETRY_TIMES;
			while(retry--)//设置激活模式
			{
				if(AT_send_cmd(AT_OTAA,ECHO_STR,200))
				{
					u2_printf("success 3\r\n");
					break;
				}
				else
				{
					u2_printf("failure 3 %01d\r\n",AT_RETRY_TIMES-retry);
				}
				if(retry==0) return 3;
			}

			retry = AT_RETRY_TIMES;
			while(retry--)//设置APPEUI
			{
				if(AT_send_cmd(AT_APPEUI,ECHO_STR,200))
				{
					u2_printf("success 4\r\n");
					break;
				}
				else
				{
					u2_printf("failure 4 %01d\r\n",AT_RETRY_TIMES-retry);
				}
				if(retry==0) return 4;
			}

			retry = AT_RETRY_TIMES;
			while(retry--)//设置APPKEY
			{
				if(AT_send_cmd(AT_APPKEY,ECHO_STR,200))
				{
					u2_printf("success 5\r\n");
					break;
				}
				else
				{
					u2_printf("failure 5 %01d\r\n",AT_RETRY_TIMES-retry);
				}
				if(retry==0) return 5;
			}

			retry = AT_RETRY_TIMES;
			while(retry--)//保存命令
			{
				if(AT_send_cmd(AT_SAVE,ECHO_STR,200))//检测串口是否准备好
				{
					u2_printf("success 6\r\n");
					break;
				}
				else
				{
					u2_printf("failure 6 %01d\r\n",AT_RETRY_TIMES-retry);
				}
				if(retry==0) return 6;
			}

			retry = AT_RETRY_TIMES;
			while(retry--)//重启模块
			{
				if(AT_send_cmd(AT_RESET,ECHO_STR,200))//检测串口是否准备好
				{
					u2_printf("success 7\r\n");
					break;
				}
				else
				{
					u2_printf("failure 7 %01d\r\n",AT_RETRY_TIMES-retry);
				}
				
				if(retry==0) return 7;
				comd_mode=0;
				LoRa_Node.NET_Mode = 1;
			}

			return 0;
	}
	if(comd_mode==0)
	{
		 static uint8_t online_log=0;
		 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
		 HAL_Delay(2);
		 HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3, GPIO_PIN_RESET);
		 HAL_Delay(10);
		if(LORANODE_STAT_STATUS == 0)
		{
			LoRa_temp->ONline = 0;
		}else
		{
			LoRa_temp->ONline = 1;
		}

		if(LORANODE_BUSY_STATUS == 0)
		{
			LoRa_temp->BUSY = 0;
		}else
		{
			LoRa_temp->BUSY = 1;
		}

		if(LoRa_temp->NET_Mode == 1)
		{
			if(LoRa_temp->ONline == 1)
			{
				if(LoRa_temp->BUSY == 1)
				{
					LoRa_temp->Join = 1;
				}
			}else
			 {
					LoRa_temp->Join = 0;
				}
		}else if(LoRa_temp->NET_Mode == 0)
			{
				if(LoRa_temp->BUSY == 1)
				{
					LoRa_temp->Join = 1;
				}else
				{
					LoRa_temp->Join = 0;
				 }
			}

			if(LoRa_temp->Join != online_log )
			{
				if(LoRa_temp->Join == 1)
				{
					printf("\r\n--> 网络已连接\r\n");
				}else
				{
						//Uart_Printf("\r\n 网关已断开连接 \r\n");
				}
			}	
			online_log=LoRa_temp->Join;
		}
	return 8;
}


