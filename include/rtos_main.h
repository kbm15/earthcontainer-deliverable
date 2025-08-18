#ifndef RTOS_MAIN_H
#define RTOS_MAIN_H

#include "cmsis_os.h"

extern osMessageQId buttonQueueHandle;
extern osThreadId buttonTaskHandle;

void rtos_main(void);

#endif 
