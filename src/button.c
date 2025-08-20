#include "button.h"
#include "cmsis_os.h"
#include "publisher.h"
#include <stdio.h>

volatile uint8_t button_state = 0;
volatile uint8_t source = 0;

void StartButtonTask(void const *argument) {
    osEvent evt;
    for (;;) {
        evt = osMessageGet(buttonQueueHandle, osWaitForever);
        if (evt.status == osEventMessage) {
            uint8_t msg_raw = (uint8_t)evt.value.v;
            // Decode source/state
            if ((msg_raw & 0xF0) == 0x00) { // EXTI
                button_state = (msg_raw == BUTTON_SRC_EXTI_PRESSED) ? 1 : 0;
                source = PUB_SRC_EXTI;
            } else if ((msg_raw & 0xF0) == 0x10) { // TIM
                button_state = (msg_raw == BUTTON_SRC_TIM_PRESSED) ? 1 : 0;
                source = PUB_SRC_TIM;
            } else {
                button_state = msg_raw; // fallback
            }
            uint8_t pub_msg = source | (msg_raw & 0x0F);
            osMessagePut(publisherQueueHandle, pub_msg, 0);
        }
        osDelay(1);
    }
}