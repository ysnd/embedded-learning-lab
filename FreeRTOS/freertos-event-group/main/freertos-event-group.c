#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"


#define SENSOR1 (1 << 0)
#define SENSOR2 (1 << 1)

EventGroupHandle_t evt_group;

void task_sensor1(void *pvParameters) {
    while (1) {
        printf("Sensor 1: Membaca Suhu...\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("Sensor 1: Selesai. Set bit 0\n");
        xEventGroupSetBits(evt_group, SENSOR1);
    }
}

void task_sensor2(void *pvParameters) {
    while (1) {
        printf("Sensor 2: Membaca tekanan...\n");
        vTaskDelay(pdMS_TO_TICKS(1500));
        printf("Sensor 2: Selesai. Set bit 1\n");
        xEventGroupSetBits(evt_group, SENSOR2);
    }
}

void task_monitor(void *pvParameters) {
    while (1) {
        printf("Menunggu semua sensor siap...\n");
        EventBits_t bits = xEventGroupWaitBits(
                evt_group,
                SENSOR1 | SENSOR2,
                pdTRUE,
                pdTRUE,
                portMAX_DELAY
                );
        if ((bits & (SENSOR1 | SENSOR2)) == (SENSOR1 | SENSOR2)) {
        printf("Semua sensor siap! Proses data...\n");
        }
    }
}

void app_main(void)
{
    evt_group = xEventGroupCreate();

    xTaskCreate(task_sensor1, "Sensor1", 2048, NULL, 2, NULL);
    xTaskCreate(task_sensor2, "SENSOR2", 2048, NULL, 2, NULL);
    xTaskCreate(task_monitor, "Monitor", 2048, NULL, 1, NULL);
}
