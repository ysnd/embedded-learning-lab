#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_rom_sys.h"


#define DHT_PIN GPIO_NUM_4

void dht_request() {
    gpio_set_direction(DHT_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(DHT_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(DHT_PIN, 1);
    esp_rom_delay_us(30);
    gpio_set_direction(DHT_PIN, GPIO_MODE_INPUT);
}

int wait_for_level(int expected_level, uint32_t timeout_us) {
    int64_t start = esp_timer_get_time();
    while (gpio_get_level(DHT_PIN) !=expected_level) {
        if ((esp_timer_get_time() - start) > timeout_us) return -1;
    }
    return 0;
}

int dht_read(uint8_t *data) {
    dht_request();

    //wait respons dht 
    if (wait_for_level(0, 80) < 0) return -1;
    if (wait_for_level(1, 80) < 0) return -1; 
    
    //read 40 bits data

    for (int i = 0; i < 40; ++i) {
        data[i]=0;
        if (wait_for_level(0, 50) < 0) return -1; 
        int64_t t_start = esp_timer_get_time();
        if (wait_for_level(1, 70) < 0) return -1;
        int64_t t_end = esp_timer_get_time();
        int bit = (t_end - t_start) > 40 ? 1 : 0;

        data[i/8] <<=1;
        data[i/8] |=bit;
        
    }
    //checksum
    uint8_t sum = data[0] + data[1] + data[2] + data[3];
    if (sum != data[4]) return -2;
    return 0;
}

void app_main(void)
{
    uint8_t data[5];

    while (1) {
        if (dht_read(data) == 0) {
            printf("Humidity: %d.%d%%\n", data[0], data[1]);
            printf("Temperature: %d.%d C\n", data[2], data[3]);
        }
        else {
        printf("Failed to read from DHT11\n");
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
