#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "hal/gpio_types.h"


#define LED1 4
#define LED2 17
#define BTN_ON 16
#define BTN_OFF 0
#define BTN_TOGGLE 2

#define DEBOUNCE_TIME_US 200000
#define HOLD_THRESHOLD_US 1000000

static int led2_status = 0;
static bool blinking = false;
static uint64_t press_start_time = 0;

QueueHandle_t gpio_evt_queue = NULL;
static uint64_t last_press_time[GPIO_NUM_MAX] = {0};

void blink_task(void* arg) {
    while(1) {
        if (blinking) {
            gpio_set_level(LED2, 1);
            vTaskDelay(pdMS_TO_TICKS(200));
            gpio_set_level(LED2, 0);
            vTaskDelay(pdMS_TO_TICKS(200));
        }
        else {
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}

//isr
static void IRAM_ATTR gpio_isr_handler(void* arg) {
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

//GPIO task
void gpio_task(void* arg) {
    uint32_t io_num;
    while (1) {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            uint64_t now = esp_timer_get_time();
            if (now - last_press_time[io_num] < DEBOUNCE_TIME_US) continue;
            last_press_time[io_num] = now;

            int level = gpio_get_level(io_num);
            printf("GPIO %lu %s\n", io_num, level == 0 ? "PRESSED" : "RELEASED");

            if (io_num == BTN_TOGGLE) {
                if (level == 0) {
                    // Tombol ditekan → simpan waktu
                    press_start_time = now;
                } else {
                    // Tombol dilepas → hitung durasi tekan
                    uint64_t held_time = now - press_start_time;
                    if (held_time >= HOLD_THRESHOLD_US) {
                        blinking = !blinking;
                        printf("LED2 blinking mode: %s\n", blinking ? "ON" : "OFF");
                    } else {
                        if (!blinking) {
                            led2_status = !led2_status;
                            gpio_set_level(LED2, led2_status);
                            printf("LED2 toggled to: %d\n", led2_status);
                        }
                    }
                }
            } else if (io_num == BTN_ON) {
                gpio_set_level(LED1, 1);
            } else if (io_num == BTN_OFF) {
                gpio_set_level(LED1, 0);
            }
        }
    }
}

void setup_gpio_input(gpio_num_t gpio) {
    gpio_reset_pin(gpio);
    gpio_set_direction(gpio, GPIO_MODE_INPUT);
    gpio_set_pull_mode(gpio, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(gpio, GPIO_INTR_ANYEDGE);

}
void app_main(void)
{
    setup_gpio_input(BTN_ON);
    setup_gpio_input(BTN_OFF);
    setup_gpio_input(BTN_TOGGLE);

    gpio_reset_pin(LED1);
    gpio_reset_pin(LED2);
    gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED2, GPIO_MODE_OUTPUT);

    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    xTaskCreate(gpio_task, "gpio_task", 2048, NULL,10 , NULL);
    xTaskCreate(blink_task, "blink_task", 2048, NULL, 5, NULL);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BTN_ON, gpio_isr_handler, (void*)BTN_ON);
    gpio_isr_handler_add(BTN_OFF, gpio_isr_handler, (void*)BTN_OFF);
    gpio_isr_handler_add(BTN_TOGGLE, gpio_isr_handler, (void*)BTN_TOGGLE);
}
