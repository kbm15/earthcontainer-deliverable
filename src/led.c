#include "cmsis_os.h"
#include "pin_alias.h"
#include "led.h"
#include "publisher.h"
#include <stdlib.h>
#include <stdio.h>


void StartLedTask(void const * argument)
{
    uint8_t receivedNumber;
    uint8_t ledState;

    for(;;)
    {
        osEvent evt = osMessageGet(randomQueueHandle, osWaitForever);
        if(evt.status == osEventMessage)
        {
            receivedNumber = evt.value.v;
            HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
            ledState = HAL_GPIO_ReadPin(LD2_GPIO_Port, LD2_Pin);
            uint8_t pub_msg = PUB_SRC_LED | (ledState & 0x0F);
            osMessagePut(publisherQueueHandle, pub_msg, 0);
            osDelay(receivedNumber * 1000); 
        }
    }
}
