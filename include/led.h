#ifndef LED_H
#define LED_H

#include "cmsis_os.h"

extern osMessageQId randomQueueHandle;

void StartLedTask(void const *argument);

#endif