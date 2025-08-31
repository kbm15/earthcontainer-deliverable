#ifndef MODEM_H
#define MODEM_H

#include "cmsis_os.h"
#include "main.h"

#define MODEM_LINE_MAX   128
#define MODEM_QUEUE_LEN  8

typedef struct {
    char line[MODEM_LINE_MAX];
} modem_line_t;

extern osMessageQId modemQueueHandle;
extern osMessageQId modemTXQueueHandle;
extern osMessageQId modemRXQueueHandle;
extern osThreadId modemTaskHandle;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;



void StartModemTask(void const * argument);

#endif // MODEM_H
