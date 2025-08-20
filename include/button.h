#ifndef BUTTON_H
#define BUTTON_H

#include "cmsis_os.h"
#include "pin_alias.h"

#define BUTTON_SRC_EXTI_PRESSED   0x00
#define BUTTON_SRC_EXTI_RELEASED  0x01
#define BUTTON_SRC_TIM_PRESSED    0x10
#define BUTTON_SRC_TIM_RELEASED   0x11

extern osMessageQId buttonQueueHandle;
extern volatile uint8_t button_state;

void StartButtonTask(void const *argument);


#endif // BUTTON_H
