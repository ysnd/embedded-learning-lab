#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

//heavy function
void heavy_computation(const char *task_name, int loop_count) {
    double result = 0;
    for (int i=0; i< loop_count; i++) {
        result += (i * 0.0001) / (i + 1);
        if (i % 100000 == 0) vTaskDelay(1);
    }
    printf("%s done. Result = %.5f\n", task_name, result);
}

//task single core
void single_core_task(void *pvParameters) {
    int loop = (int)(intptr_t)pvParameters;
    int64_t start = esp_timer_get_time();
    heavy_computation("TASK A", loop);
    heavy_computation("TASK B", loop);

    int64_t end = esp_timer_get_time();
    printf("Single Core runtime: %.2f ms\n", (end - start) / 1000.0);
    vTaskDelete(NULL);
}

void core0_task(void *pvParameters) {
    heavy_computation("Task A (core0)", (int)pvParameters);
    vTaskDelete(NULL);
}

void core1_task(void *pvParameters) {
    heavy_computation("Task B (Core1)", (int)pvParameters);
    vTaskDelete(NULL);
}

void app_main(void)
{
    int loop_count = (int)1e7;
    printf("\n==== [1] Single Core Test ====\n");
    xTaskCreate(single_core_task, "singlecore", 4096, (void *)loop_count, 5, NULL);
    // wait until done.
    vTaskDelay(pdMS_TO_TICKS(5000));
    printf("\n==== [2] Dual Core Test ====\n");
    int64_t start = esp_timer_get_time();

    //run task in both core.
    xTaskCreatePinnedToCore(core0_task, "core0task", 4096, (void *)loop_count, 5, NULL, 0);
    xTaskCreatePinnedToCore(core1_task, "core1task", 4096, (void *)loop_count, 5, NULL, 1);
    //wait until task done
    vTaskDelay(pdMS_TO_TICKS(5000));
    int64_t end = esp_timer_get_time();
    printf("Dual-core runtime (approx): %.2f ms\n", (end - start)/1000.0);
}
