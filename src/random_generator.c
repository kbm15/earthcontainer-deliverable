#include "cmsis_os.h"
#include "pin_alias.h"
#include "random_generator.h"
#include "publisher.h"
#include <stdio.h>

void StartRandomTask(void const * argument)
{
    uint8_t randomNumber = 0;

    for(;;)
    {
		randomNumber = (rand() % 5) + 1;
		uint8_t pub_msg = PUB_SRC_RANDOM | (randomNumber & 0x0F);
		osMessagePut(publisherQueueHandle, pub_msg, 0);
		osMessagePut(randomQueueHandle, randomNumber, 0);
		osDelay(randomNumber * 1000);
    }
}
