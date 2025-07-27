#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "esp_log.h"


#define BTN 0
#define LED 2
#define ESP_INTR_FLAG_DEFAULT 0

#define BIT_BTN_PRESS (1 << 0)
#define BIT_TIMER_DONE (1 << 1)

static EventGroupHandle_t event_group;
static TimerHandle_t my_timer;
static const char *TAG = "Experiment";

static void IRAM_ATTR gpio_isr_handler(void* arg) {
    xEventGroupSetBitsFromISR(event_group, BIT_BTN_PRESS, NULL);
}

void timer_callback(TimerHandle_t xTimer) {
    ESP_LOGI(TAG, "Timer selesai");
    xEventGroupSetBits(event_group, BIT_TIMER_DONE);
}

void led_task(void *arg) {
    while (1) {
        EventBits_t bits = xEventGroupWaitBits(event_group, 
                BIT_BTN_PRESS | BIT_TIMER_DONE, 
                pdTRUE, 
                pdFALSE, 
                portMAX_DELAY
                );
        if (bits & BIT_BTN_PRESS) {
            ESP_LOGI(TAG, "Tombol ditekan, mulai timer");
            xTimerStart(my_timer, 0);
        }
        if (bits & BIT_TIMER_DONE) {
            ESP_LOGI(TAG, "LED dinyalakan");
            gpio_set_level(LED, 1);
            vTaskDelay(pdMS_TO_TICKS(1000));
            gpio_set_level(LED, 0);
        }
    }
}
    

void app_main(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BTN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 1,
        .intr_type = GPIO_INTR_POSEDGE
    };
    gpio_config(&io_conf);

    gpio_config_t led_conf = {
        .pin_bit_mask = (1ULL << LED),
        .mode = GPIO_MODE_OUTPUT
    };
    gpio_config(&led_conf);

    event_group = xEventGroupCreate();

    my_timer = xTimerCreate("mytimer", pdMS_TO_TICKS(5000), pdTRUE, NULL, timer_callback);

    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(BTN, gpio_isr_handler, NULL);

    xTaskCreate(led_task, "led_task", 2048, NULL, 5, NULL);
    ESP_LOGI(TAG, "System Initialized");
}
