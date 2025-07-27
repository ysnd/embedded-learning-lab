#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "driver/gpio.h"


#define BTN 0
#define LED 2

static QueueHandle_t btn_queue;
static SemaphoreHandle_t btn_semaphore;
static TimerHandle_t debounce_timer;

static void IRAM_ATTR btn_isr_handler(void* arg) {
    int btn_val = 1;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xQueueSendFromISR(btn_queue, &btn_val, &xHigherPriorityTaskWoken);
    xSemaphoreGiveFromISR(btn_semaphore, &xHigherPriorityTaskWoken);
    xTimerStartFromISR(debounce_timer, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

void debounce_timer_callback(TimerHandle_t xTimer) {
    printf("Debounce timer triggered, tombol diproses\n");
}

void task_led_handler(void* arg) {
    uint32_t received;
    while (1) {
        if (xQueueReceive(btn_queue, &received, portMAX_DELAY)) {
            gpio_set_level(LED, 1);
            vTaskDelay(pdMS_TO_TICKS(200));
            gpio_set_level(LED, 0);
        }
    
    }
}

void semaphore_task(void *arg) {
    while (1) {
        if (xSemaphoreTake(btn_semaphore, portMAX_DELAY)) {
            printf("Semaphore dari tombol diterima\n");
        }
    }
}

void app_main(void)
{
    gpio_set_direction(LED,GPIO_MODE_OUTPUT);
    gpio_set_direction(BTN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BTN, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(BTN, GPIO_INTR_NEGEDGE);

    btn_queue = xQueueCreate(10, sizeof(uint32_t));
    btn_semaphore = xSemaphoreCreateBinary();

    debounce_timer = xTimerCreate("debounce_timer", pdMS_TO_TICKS(500), pdFALSE, NULL, debounce_timer_callback);


    xTaskCreate(task_led_handler, "Led Task", 2048, NULL, 10, NULL);
    xTaskCreate(semaphore_task, "Semaphote task", 2048, NULL, 10, NULL);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BTN, btn_isr_handler, NULL);
    printf("Sistem siap. tekan tombol untuk menguji isr ->queue+semaphore\n");
}
