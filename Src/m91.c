#include "m91.h"
#include "lpuart1.h"
#include "usart2.h"
#include "gpio.h"
#include "string.h"
uint8_t  comd_mode =1;
Node_Info LoRa_Node;
Node_Info *LoRa_temp = &LoRa_Node;
//waittime�������1����λ10ms
//����ֵ��1-�ɹ���0-ʧ��
uint8_t AT_send_cmd(char *cmd,char *ack,uint16_t waittime)
{
	uint8_t res = 1;//����Ӧ��ʱĬ�Ϸ���ֵΪ1���ɹ���

	u2_printf("%s\r\n",cmd);//��������

	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(waittime)	//�ȴ�����ʱ
		{
			if(strstr((const char*)USART2_RX_BUF,(const char*)ack))
			{
				break;//�õ���Ч����
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

//����ֵ��8-�ɹ�����0-�������
uint8_t M91_Init(void)
{
	uint8_t retry;
	if(comd_mode==1)
	{
			retry = AT_RETRY_TIMES;
			while(retry--)//��⴮���Ƿ�׼����
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
			while(retry--)//��������
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
			while(retry--)//���ü���ģʽ
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
			while(retry--)//����APPEUI
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
			while(retry--)//����APPKEY
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
			while(retry--)//��������
			{
				if(AT_send_cmd(AT_SAVE,ECHO_STR,200))//��⴮���Ƿ�׼����
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
			while(retry--)//����ģ��
			{
				if(AT_send_cmd(AT_RESET,ECHO_STR,200))//��⴮���Ƿ�׼����
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
					printf("\r\n--> ����������\r\n");
				}else
				{
						//Uart_Printf("\r\n �����ѶϿ����� \r\n");
				}
			}	
			online_log=LoRa_temp->Join;
		}
	return 8;
}


