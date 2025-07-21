#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"


#define LED GPIO_NUM_2
#define BTN GPIO_NUM_0

SemaphoreHandle_t binary_semaphore = NULL;

static void IRAM_ATTR button_isr_handler(void* arg) {
    xSemaphoreGiveFromISR(binary_semaphore, NULL);
}

void led_task(void *param) {
    while (1) {
        if (xSemaphoreTake(binary_semaphore, portMAX_DELAY)) {
            printf("Button pressed! LED ON\n");
            gpio_set_level(LED, 1);
            vTaskDelay(pdMS_TO_TICKS(500));
            gpio_set_level(LED, 0);
        }
    }
}

void app_main(void)
{
    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    gpio_reset_pin(BTN);
    gpio_set_direction(BTN, GPIO_MODE_INPUT);
    gpio_set_intr_type(BTN, GPIO_INTR_NEGEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BTN, button_isr_handler, NULL);

    //buat binary Semaphore 
    binary_semaphore = xSemaphoreCreateBinary();

    xTaskCreate(led_task, "led_task", 2048, NULL, 10, NULL);
}
