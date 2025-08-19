#include "cmsis_os.h"
#include "button.h"
#include "led.h"
#include "random_generator.h"

osMessageQId buttonQueueHandle;
osMessageQId randomQueueHandle;
osThreadId buttonTaskHandle;
osThreadId ledTaskHandle;
osThreadId randomTaskHandle;


void rtos_main(void)
{
    osMessageQDef(buttonQueue, 5, uint8_t);
    buttonQueueHandle = osMessageCreate(osMessageQ(buttonQueue), NULL);

    osThreadDef(buttonTask, StartButtonTask, osPriorityNormal, 0, 128);
    buttonTaskHandle = osThreadCreate(osThread(buttonTask), NULL);

    osMessageQDef(randomQueue, 5, uint8_t);
    randomQueueHandle = osMessageCreate(osMessageQ(randomQueue), NULL);

    osThreadDef(ledTask, StartLedTask, osPriorityLow, 0, 128);
    ledTaskHandle = osThreadCreate(osThread(ledTask), NULL);

    osThreadDef(randomTask, StartRandomTask, osPriorityLow, 0, 128);
    randomTaskHandle = osThreadCreate(osThread(randomTask), NULL);

    osKernelStart();

    while (1)
    {
    }
}