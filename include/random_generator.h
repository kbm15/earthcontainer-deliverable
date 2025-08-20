#ifndef RANDOM_GENERATOR_H
#define RANDOM_GENERATOR_H

#include "cmsis_os.h"
#include <stdint.h>
#include <stdlib.h>

/* Queue handle to send numbers to LED task */
extern osMessageQId randomQueueHandle;

/* Task function prototype */
void StartRandomTask(void const * argument);

#endif /* RANDOM_GENERATOR_H */