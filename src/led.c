#include "cmsis_os.h"
#include "pin_alias.h"
#include "led.h"
#include <stdlib.h>


void StartLedTask(void const * argument)
{
    uint8_t receivedNumber;
    uint8_t ledState;
    

    for(;;)
    {
        if(osMessageGet(randomQueueHandle, osWaitForever).status == osEventMessage)
        {
            receivedNumber = osMessageGet(randomQueueHandle, 0).value.v;
            HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
            ledState = HAL_GPIO_ReadPin(LD2_GPIO_Port, LD2_Pin);

            osDelay(receivedNumber * 1000); 
        }
    }
}