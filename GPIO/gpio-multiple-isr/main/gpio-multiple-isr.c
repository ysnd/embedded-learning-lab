#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#define LED_GPIO    4
#define BTN_ON      16
#define BTN_OFF     0
#define BTN_TOGGLE  2

QueueHandle_t gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg) {
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

void gpio_task(void* arg) {
    uint32_t io_num;
    while (1) {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            printf("Interrupt dari GPIO %lu\n", io_num);
            switch (io_num) {
                case BTN_ON:
                    gpio_set_level(LED_GPIO, 1);
                    break;
                case BTN_OFF:
                    gpio_set_level(LED_GPIO, 0);
                    break;
                case BTN_TOGGLE:
                    gpio_set_level(LED_GPIO, !gpio_get_level(LED_GPIO));
                    break;
                default:
                    break;
            }
        }
    }
}

void setup_gpio_input(gpio_num_t gpio) {
    gpio_reset_pin(gpio);
    gpio_set_direction(gpio, GPIO_MODE_INPUT);
    gpio_set_pull_mode(gpio, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(gpio, GPIO_INTR_NEGEDGE); // Aktif saat tombol ditekan
}

void app_main(void) {
    setup_gpio_input(BTN_ON);
    setup_gpio_input(BTN_OFF);
    setup_gpio_input(BTN_TOGGLE);

    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(gpio_task, "gpio_task", 2048, NULL, 10, NULL);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BTN_ON, gpio_isr_handler, (void*) BTN_ON);
    gpio_isr_handler_add(BTN_OFF, gpio_isr_handler, (void*) BTN_OFF);
    gpio_isr_handler_add(BTN_TOGGLE, gpio_isr_handler, (void*) BTN_TOGGLE);
}
