#include <stdio.h>
#include "esp_err.h"
#include "esp_spiffs.h"
#include "dht11.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    //mount SPIFFS  
    esp_vfs_spiffs_conf_t c = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&c);
    if (ret != ESP_OK) {
        printf("Failed moount SPIFFS (%s)\n", esp_err_to_name(ret));
        return;
    }
    //init sensor
    dht11_init(GPIO_NUM_4);
    dht11_data_t data;
// loop read data
    while (1) {
        if (dht11_read(&data)) {
            printf("Temperature: %.1f C, Humidity: %.1f%%\n", data.temperature, data.humidity);
        //open file for append
            FILE *f = fopen("/spiffs/dht11_log.csv", "a");
            if (f==NULL) {
                printf("Failed to open file writing!\n");
            } else {
                fprintf(f, "%.1f,%.1f\n", data.temperature, data.humidity);
                fclose(f);
                printf("Data store complete succesfully to log\n");
            }
        } else {
            printf("failed read data from dht11!\n");
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
    esp_vfs_spiffs_unregister(NULL);
}
