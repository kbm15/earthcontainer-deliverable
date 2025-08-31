#include "modem.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern UART_HandleTypeDef huart2;

void StartModemTask(void const *argument)
{
	enum
	{
		MODEM_WAIT_AT,
		MODEM_CONFIG_HTTP,
		MODEM_WAIT_OK,
		MODEM_REG_CHECK,
		MODEM_WAIT_COPS,
		MODEM_READY
	} modem_state = MODEM_WAIT_AT;

	int registered = 0;
	char tx_buf[256];
	char debug_buf[256];

	for (;;)
	{
		switch (modem_state)
		{
		case MODEM_WAIT_AT:
			strcpy(tx_buf, "ATE0\r");
			HAL_UART_Transmit(&huart1, (uint8_t *)tx_buf, strlen(tx_buf), 100);
			snprintf(debug_buf, sizeof(debug_buf), "[MODEM] MODEM_WAIT_AT : Sending command: %s\r\n", "ATE0");
			HAL_UART_Transmit(&huart2, (uint8_t *)debug_buf, strlen(debug_buf), 100);

			break;
		case MODEM_CONFIG_HTTP:
			strcpy(tx_buf, "AT%HTTPCFG=\"NODES\",1,\"http://40.66.43.54/telemetry\",\"\",\"\",0\r");
			HAL_UART_Transmit(&huart1, (uint8_t *)tx_buf, strlen(tx_buf), 100);
			snprintf(debug_buf, sizeof(debug_buf), "[MODEM] MODEM_CONFIG_HTTP : Sending HTTP config command\r\n");
			HAL_UART_Transmit(&huart2, (uint8_t *)debug_buf, strlen(debug_buf), 100);

			break;
		case MODEM_REG_CHECK:
			strcpy(tx_buf, "AT+CEREG?\r");
			HAL_UART_Transmit(&huart1, (uint8_t *)tx_buf, strlen(tx_buf), 100);
			snprintf(debug_buf, sizeof(debug_buf), "[MODEM] MODEM_REG_CHECK : Sending CEREG query\r\n");
			HAL_UART_Transmit(&huart2, (uint8_t *)debug_buf, strlen(debug_buf), 100);

			break;
		case MODEM_WAIT_COPS:
			strcpy(tx_buf, "AT+COPS?\r");
			HAL_UART_Transmit(&huart1, (uint8_t *)tx_buf, strlen(tx_buf), 100);
			snprintf(debug_buf, sizeof(debug_buf), "[MODEM] MODEM_WAIT_COPS : Sending COPS query\r\n");
			HAL_UART_Transmit(&huart2, (uint8_t *)debug_buf, strlen(debug_buf), 100);

			break;
		case MODEM_READY:
			// Wait for publisher messages
			{
				osEvent pub_evt = osMessageGet(modemTXQueueHandle, 5000);
				if (pub_evt.status == osEventMessage)
				{
					char *json_msg = (char *)pub_evt.value.p;
					// Compose HTTPSEND command
					snprintf(tx_buf, sizeof(tx_buf), "AT%%HTTPSEND=\"POST\",1,%d\r", (int)strlen(json_msg));
					HAL_UART_Transmit(&huart1, (uint8_t *)tx_buf, strlen(tx_buf), 100);
					HAL_UART_Transmit(&huart1, (uint8_t *)json_msg, strlen(json_msg), 100);
					snprintf(debug_buf, sizeof(debug_buf), "[MODEM] MODEM_READY : Sending JSON Document\r\n");
					HAL_UART_Transmit(&huart2, (uint8_t *)debug_buf, strlen(debug_buf), 100);
					snprintf(debug_buf, sizeof(debug_buf), "[MODEM] MODEM_READY : %s\n", json_msg);
					HAL_UART_Transmit(&huart2, (uint8_t *)debug_buf, strlen(debug_buf), 100);
					free(json_msg);
				}
				else
				{
					snprintf(debug_buf, sizeof(debug_buf), "[MODEM] TX : No message received within timeout\r\n");
					HAL_UART_Transmit(&huart2, (uint8_t *)debug_buf, strlen(debug_buf), 100);
				}
			}
			break;
		default:
			break;
		}

		osEvent evt = osMessageGet(modemRXQueueHandle, 5000);
		if (evt.status == osEventMessage)
		{
			modem_line_t *rx_line = (modem_line_t *)evt.value.p;
			if (rx_line)
			{

				switch (modem_state)
				{
				case MODEM_WAIT_AT:
					if (strstr(rx_line->line, "ATE0"))
					{
						modem_state = MODEM_CONFIG_HTTP;
						snprintf(debug_buf, sizeof(debug_buf), "[MODEM] RX : %s", rx_line->line);
						HAL_UART_Transmit(&huart2, (uint8_t *)debug_buf, strlen(debug_buf), 100);
						free(rx_line);
						evt = osMessageGet(modemRXQueueHandle, 100);
					}
					else
					{
						if (strstr(rx_line->line, "OK"))
						{
							modem_state = MODEM_CONFIG_HTTP;
							snprintf(debug_buf, sizeof(debug_buf), "[MODEM] RX : %s", rx_line->line);
							HAL_UART_Transmit(&huart2, (uint8_t *)debug_buf, strlen(debug_buf), 100);
						}
						else
						{
							snprintf(debug_buf, sizeof(debug_buf), "[MODEM] RX error: %s", rx_line->line);
							HAL_UART_Transmit(&huart2, (uint8_t *)debug_buf, strlen(debug_buf), 100);
							osDelay(1000);
						}
					}
					break;
				case MODEM_CONFIG_HTTP:
					if (strstr(rx_line->line, "OK"))
					{
						modem_state = MODEM_REG_CHECK;
						snprintf(debug_buf, sizeof(debug_buf), "[MODEM] RX : %s", rx_line->line);
						HAL_UART_Transmit(&huart2, (uint8_t *)debug_buf, strlen(debug_buf), 100);
					}
					else
					{

							snprintf(debug_buf, sizeof(debug_buf), "[MODEM] RX error: %s", rx_line->line);
							HAL_UART_Transmit(&huart2, (uint8_t *)debug_buf, strlen(debug_buf), 100);


					}
					break;

					break;
				case MODEM_REG_CHECK:
					if (strstr(rx_line->line, "+CEREG:"))
					{
						char *cereg = strstr(rx_line->line, "+CEREG:");
						int stat = 0;
						char *comma = strchr(cereg, ',');
						if (comma && *(comma + 1))
						{
							stat = atoi(comma + 1);
							if (stat == 1 || stat == 5)
							{
								registered = 1;
							}
						}
						if (registered)
						{
							modem_state = MODEM_WAIT_COPS;
							snprintf(debug_buf, sizeof(debug_buf), "[MODEM] RX : %s", rx_line->line);
							HAL_UART_Transmit(&huart2, (uint8_t *)debug_buf, strlen(debug_buf), 100);
						}
						else
						{
							modem_state = MODEM_REG_CHECK;
							snprintf(debug_buf, sizeof(debug_buf), "[MODEM] Waiting for network registration\r\n");
							HAL_UART_Transmit(&huart2, (uint8_t *)debug_buf, strlen(debug_buf), 100);
							osDelay(5000);
						}
						free(rx_line);
						evt = osMessageGet(modemRXQueueHandle, 100);
					}
					else
					{
							snprintf(debug_buf, sizeof(debug_buf), "[MODEM] RX error: %s", rx_line->line);
							HAL_UART_Transmit(&huart2, (uint8_t *)debug_buf, strlen(debug_buf), 100);
							osDelay(1000);

					}
					break;
				case MODEM_WAIT_COPS:
					if (strstr(rx_line->line, "+COPS:"))
					{
						modem_state = MODEM_READY;
						snprintf(debug_buf, sizeof(debug_buf), "[MODEM] RX : %s", rx_line->line);
						HAL_UART_Transmit(&huart2, (uint8_t *)debug_buf, strlen(debug_buf), 100);
						free(rx_line);
						evt = osMessageGet(modemRXQueueHandle, 100);
					}
					else
					{
							snprintf(debug_buf, sizeof(debug_buf), "[MODEM] RX error: %s", rx_line->line);
							HAL_UART_Transmit(&huart2, (uint8_t *)debug_buf, strlen(debug_buf), 100);
							osDelay(1000);

					}
					break;
				case MODEM_READY:
					if (strstr(rx_line->line, "OK"))
					{
						snprintf(debug_buf, sizeof(debug_buf), "[MODEM] JSON successfully sent\r\n");
						HAL_UART_Transmit(&huart2, (uint8_t *)debug_buf, strlen(debug_buf), 100);
					}

					break;

				default:
					snprintf(debug_buf, sizeof(debug_buf), "[MODEM] RX : Unexpected message received\r\n");
					HAL_UART_Transmit(&huart2, (uint8_t *)debug_buf, strlen(debug_buf), 100);

					break;
				}
				free(rx_line);
			}
		}
		else
		{
			snprintf(debug_buf, sizeof(debug_buf), "[MODEM] RX : No message received within timeout\r\n");
			HAL_UART_Transmit(&huart2, (uint8_t *)debug_buf, strlen(debug_buf), 100);
		}
	}
}
