#include "cmsis_os.h"
#include "pin_alias.h"
#include "random_generator.h"
       

void StartRandomTask(void const * argument)
{
    uint8_t randomNumber;

    for(;;)
    {
        randomNumber = (rand() % 5) + 1; // 1..5
        osMessagePut(randomQueueHandle, randomNumber, 0);
        osDelay(randomNumber * 1000); 
    }
}