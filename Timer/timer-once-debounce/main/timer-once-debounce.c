#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"


#define BTN 0
#define LED 2
#define DEBOUNCE_TIME_MS 50

static const char *TAG = "DEBOUNCE";
static esp_timer_handle_t debounce_timer;
static volatile bool debounce_in_progress = false;

static void IRAM_ATTR button_isr_handler(void* arg) {
    if (!debounce_in_progress) {
        debounce_in_progress = true;
        esp_timer_start_once(debounce_timer, DEBOUNCE_TIME_MS * 1000);
    }
}

void debounce_timer_callback(void* arg) {
    int level = gpio_get_level(BTN);
    if (level == 0) {
        int led_state = gpio_get_level(LED);
        gpio_set_level(LED, !led_state);
        ESP_LOGI(TAG, "Tombol ditekan, LED di-toggle");
    }
    debounce_in_progress = false;
}

void app_main(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BTN),
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_NEGEDGE,
        .pull_up_en = GPIO_PULLUP_ENABLE
    };
    gpio_config(&io_conf);

    gpio_config_t led_conf = {
        .pin_bit_mask = (1ULL << LED),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&led_conf);

    esp_timer_create_args_t debounce_timer_args = {
        .callback = debounce_timer_callback,
        .name = "debounce_timer"
    };
    esp_timer_create(&debounce_timer_args, &debounce_timer);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BTN, button_isr_handler, NULL);

    ESP_LOGI(TAG, "Sistem siap. Tekan tombol untuk togle LED");

}
