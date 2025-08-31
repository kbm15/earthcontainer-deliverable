#ifndef PUBLISHER_H
#define PUBLISHER_H

#include "cmsis_os.h"
#include "pin_alias.h"
#include <stdint.h>

// Source masks
#define PUB_SRC_BUTTON  0x10
#define PUB_SRC_RANDOM  0x20
#define PUB_SRC_LED     0x30
#define PUB_SRC_TIM     0x40
#define PUB_SRC_EXTI    0x50

#define DEVICE_ID "potato1"

extern osMessageQId publisherQueueHandle;
extern osMessageQId modemTXQueueHandle;
extern osThreadId publisherTaskHandle;
extern UART_HandleTypeDef huart2;


void StartPublisherTask(void const * argument);

#endif // PUBLISHER_H
