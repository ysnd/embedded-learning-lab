#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "dht11.h"


#define DHT 4
#define LED 2
#define THRES 30.f  

//Queue handle
static QueueHandle_t temp_queue;
//sensor task in core0
void sensor_task(void *pvParameters) {
    dht11_data_t data;
    dht11_init(DHT);

    while (1) {
        if (dht11_read(&data)) {
            printf("[Core 0] Temp: %.1f C | Humidity: %.1f%%\n", data.temperature, data.humidity);
            //send to Queue
            xQueueSend(temp_queue, &data.temperature, portMAX_DELAY);
        } else {
            printf("[Core 0] failed to read sensor!\n");
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

//control led task in core1 
void control_task(void *pvParameters) {
    float received_temp = 0.0f;
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    while (1) {
        if (xQueueReceive(temp_queue, &received_temp, portMAX_DELAY)) {
            printf("[Core 1] Received temp: %.1f C\n", received_temp);

            if (received_temp > THRES) {
                gpio_set_level(LED, 1);
                printf("[Core 1] LED ON (High Temp)\n");
            } else {
                gpio_set_level(LED, 0);
                printf("[Core 1] LED OFF\n");
            }
        }
    }
}

void app_main(void)
{
    printf("\n===== Dual-core Sensor control system=====\n");

    //create Queue
    temp_queue = xQueueCreate(5, sizeof(float));

    //run task in different task    
    xTaskCreatePinnedToCore(sensor_task, "sensor_task", 4096, NULL, 4, NULL, 0);
    xTaskCreatePinnedToCore(control_task, "control_task", 2048, NULL, 4, NULL, 1);
}
