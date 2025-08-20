#include "cmsis_os.h"
#include "publisher.h"
#include <string.h>
#include <stdio.h>


void StartPublisherTask(void const * argument)
{
    char json_buf[128];
    const char* device_id = DEVICE_ID;
    static uint8_t last_led_state = 0;
    static uint8_t last_random_delay = 0;
    static uint8_t last_button_state = 0;

    snprintf(json_buf, sizeof(json_buf),
        "Started Publisher task\r\n"
        );
    HAL_UART_Transmit(&huart2, (uint8_t*)json_buf, strlen(json_buf), 100);


    for(;;)
    {
        osEvent evt = osMessageGet(publisherQueueHandle, osWaitForever);
        if (evt.status == osEventMessage) {
            uint8_t msg = (uint8_t)evt.value.v;
            uint8_t source = msg & 0xF0;
            uint8_t value = msg & 0x0F;
            switch(source) {
                case PUB_SRC_RANDOM:
                    last_random_delay = value;
                    snprintf(json_buf, sizeof(json_buf),
                        "{\"device_id\":\"%s\",\"random_delay\":%d}\r\n",
                        device_id, last_random_delay);
                    HAL_UART_Transmit(&huart2, (uint8_t*)json_buf, strlen(json_buf), 100);
                    break;
                case PUB_SRC_LED:
                    last_led_state = value;
                    break;
                case PUB_SRC_TIM:
                    last_button_state = value;
                    snprintf(json_buf, sizeof(json_buf),
                        "{\"device_id\":\"%s\",\"button_state\":%d}\r\n",
                        device_id, last_button_state);
                    HAL_UART_Transmit(&huart2, (uint8_t*)json_buf, strlen(json_buf), 100);
                    break;
                case PUB_SRC_EXTI:
                    last_button_state = value;
                    snprintf(json_buf, sizeof(json_buf),
                        "{\"device_id\":\"%s\",\"button_state\":%d,\"led_state\":%d,\"random_delay\":%d}\r\n",
                        device_id, last_button_state, last_led_state, last_random_delay);
                    HAL_UART_Transmit(&huart2, (uint8_t*)json_buf, strlen(json_buf), 100);
                    break;
                default:
                    break;
            }
        }
    }
}
