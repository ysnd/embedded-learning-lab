#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"


#define LED GPIO_NUM_2
#define BTN GPIO_NUM_0

SemaphoreHandle_t counting_semaphore;

void IRAM_ATTR button_isr_handler(void *arg) {
    xSemaphoreGiveFromISR(counting_semaphore, NULL);
}

void led_task(void *param) {
    int press_count = 0;
    while (1) {
        if (xSemaphoreTake(counting_semaphore, portMAX_DELAY) == pdTRUE) {
            press_count++;
            printf("Button Pressed %d times\n", press_count);
            if (press_count >=3 ) {
                gpio_set_level(LED, 1);
                vTaskDelay(pdMS_TO_TICKS(1000));
                gpio_set_level(LED, 0);
            }
        }
    }
}


void app_main(void)
{
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_direction(BTN, GPIO_MODE_INPUT);
    gpio_set_intr_type(BTN, GPIO_INTR_NEGEDGE);

    counting_semaphore = xSemaphoreCreateCounting(10, 0);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BTN, button_isr_handler, NULL);

    xTaskCreate(led_task, "led_task", 2048, NULL, 1, NULL);

}
