#include "modem.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>



extern UART_HandleTypeDef huart2;

void StartModemTask(void const * argument)
{
	enum {
		MODEM_WAIT_AT,
		MODEM_CONFIG_HTTP,
		MODEM_WAIT_HTTP_OK,
		MODEM_REG_CHECK,
		MODEM_WAIT_COPS,
		MODEM_READY
	} modem_state = MODEM_WAIT_AT;

	int registered = 0;
	char tx_buf[128];

	for (;;) {
		switch (modem_state) {
			case MODEM_WAIT_AT:
				strcpy(tx_buf, "AT\r");
				HAL_UART_Transmit(&huart1, (uint8_t*)tx_buf, strlen(tx_buf), 100);
				HAL_UART_Transmit(&huart2, (uint8_t*)tx_buf, strlen(tx_buf), 100);
				break;
			case MODEM_CONFIG_HTTP:
				strcpy(tx_buf, "AT%HTTPCFG=\"NODES\",1,\"http://40.66.43.54/telemetry\",\"\",\"\",0\r");
				HAL_UART_Transmit(&huart1, (uint8_t*)tx_buf, strlen(tx_buf), 100);
				HAL_UART_Transmit(&huart2, (uint8_t*)tx_buf, strlen(tx_buf), 100);
				break;
			case MODEM_REG_CHECK:
				strcpy(tx_buf, "AT+CEREG?\r");
				HAL_UART_Transmit(&huart1, (uint8_t*)tx_buf, strlen(tx_buf), 100);
				HAL_UART_Transmit(&huart2, (uint8_t*)tx_buf, strlen(tx_buf), 100);
				break;
			case MODEM_WAIT_COPS:
				strcpy(tx_buf, "AT+COPS?\r");
				HAL_UART_Transmit(&huart1, (uint8_t*)tx_buf, strlen(tx_buf), 100);
				HAL_UART_Transmit(&huart2, (uint8_t*)tx_buf, strlen(tx_buf), 100);
				break;
			case MODEM_READY:
				// Wait for publisher messages
				{
					osEvent pub_evt = osMessageGet(modemTXQueueHandle, 100);
					if (pub_evt.status == osEventMessage) {
						char *json_msg = (char*)pub_evt.value.p;
						// Compose HTTPSEND command
						snprintf(tx_buf, sizeof(tx_buf), "AT%%HTTPSEND=\"POST\",1,%d\r", (int)strlen(json_msg));
						HAL_UART_Transmit(&huart1, (uint8_t*)tx_buf, strlen(tx_buf), 100);
						HAL_UART_Transmit(&huart2, (uint8_t*)tx_buf, strlen(tx_buf), 100);
						HAL_UART_Transmit(&huart1, (uint8_t*)json_msg, strlen(json_msg), 100);
						HAL_UART_Transmit(&huart2, (uint8_t*)json_msg, strlen(json_msg), 100);
						free(json_msg);
					}
				}
				break;
			default:
				break;
		}

		osEvent evt = osMessageGet(modemRXQueueHandle, 100);
		if (evt.status == osEventMessage) {
			modem_line_t *rx_line = (modem_line_t*)evt.value.p;
			if (rx_line) {
				HAL_UART_Transmit(&huart2, (uint8_t*)rx_line->line, strlen(rx_line->line), 100);

				switch (modem_state) {
					case MODEM_WAIT_AT:
						if (strstr(rx_line->line, "OK")) {
							modem_state = MODEM_CONFIG_HTTP;
						}
						break;
					case MODEM_CONFIG_HTTP:
						if (strstr(rx_line->line, "OK")) {
							modem_state = MODEM_REG_CHECK;
						}
						break;
					case MODEM_REG_CHECK:
						if (strstr(rx_line->line, "+CEREG:")) {
							char *cereg = strstr(rx_line->line, "+CEREG:");
							int stat = 0;
							char *comma = strchr(cereg, ',');
							if (comma && *(comma+1)) {
								stat = atoi(comma+1);
								if (stat == 1 || stat == 5) {
									registered = 1;
								}
							}
						}
						if (strstr(rx_line->line, "OK")) {
							if (registered) {
								modem_state = MODEM_WAIT_COPS;
							} else {
								modem_state = MODEM_REG_CHECK;
							}
						}
						break;
					case MODEM_WAIT_COPS:
						if (strstr(rx_line->line, "+COPS:")) {
							// Operator info received
						}
						if (strstr(rx_line->line, "OK")) {
							modem_state = MODEM_READY;
						}
						break;
					default:
						break;
				}
				free(rx_line);
			}
		}
	}
}

