#include "cmsis_os.h"
#include "button.h"

osMessageQId buttonQueueHandle;
osThreadId buttonTaskHandle;


void rtos_main(void)
{
    osMessageQDef(buttonQueue, 5, uint8_t);
    buttonQueueHandle = osMessageCreate(osMessageQ(buttonQueue), NULL);
    osThreadDef(buttonTask, StartButtonTask, osPriorityNormal, 0, 128);
    buttonTaskHandle = osThreadCreate(osThread(buttonTask), NULL);


    osKernelStart();

    while (1)
    {
    }
}