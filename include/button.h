#ifndef BUTTON_H
#define BUTTON_H

#include "cmsis_os.h"
#include "pin_alias.h"

extern osMessageQId buttonQueueHandle;
extern volatile uint8_t button_state;

void StartButtonTask(void const *argument);


#endif // BUTTON_H
