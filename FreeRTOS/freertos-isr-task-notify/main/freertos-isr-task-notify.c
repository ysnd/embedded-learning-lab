#include <stdio.h>
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "freertos/portmacro.h"
#include "driver/gpio.h"
#include "esp_log.h"


#define BTN 0
#define LED 2

static const char *TAG = "NotifyISR";

TaskHandle_t led_task_handle = NULL;

static void IRAM_ATTR gpio_isr_handler(void *arg) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    vTaskNotifyGiveFromISR(led_task_handle, &xHigherPriorityTaskWoken);

    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

void led_task(void *arg) {
    while (1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        gpio_set_level(LED, 1);
        ESP_LOGI(TAG, "LED ON karena tombol ditekan");
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_set_level(LED, 0);
        ESP_LOGI(TAG, "LED OFF");
    }
}



void app_main(void)
{
    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    gpio_reset_pin(BTN);
    gpio_set_direction(BTN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BTN, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(BTN, GPIO_INTR_NEGEDGE);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BTN, gpio_isr_handler, NULL);

    xTaskCreate(led_task, "LED Task", 2048, NULL, 10, &led_task_handle);

}
