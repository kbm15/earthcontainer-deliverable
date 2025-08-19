#include "button.h"
#include "cmsis_os.h"

volatile uint8_t button_state = 0;

void StartButtonTask(void const *argument) {
    osEvent evt;
    for (;;) {
        evt = osMessageGet(buttonQueueHandle, osWaitForever);
        if (evt.status == osEventMessage) {
            button_state = (uint8_t)evt.value.v;
            HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin); 
        }
        osDelay(1);
    }
}