#include <stdio.h>
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"

#define DHT_PIN 4

void read_dht() {
    uint8_t data[5] = {0};
    // kirim start sinyal
    gpio_set_direction(DHT_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(DHT_PIN, 0);// 
    esp_rom_delay_us(18000); //tahan low 40us
    gpio_set_level(DHT_PIN, 1);
    esp_rom_delay_us(40); //tahan high 40us
    
    //menyiapkan baca data
    gpio_set_direction(DHT_PIN, GPIO_MODE_INPUT);

    //tunggu respon sinyal (80us low + 80us high)
    while (gpio_get_level(DHT_PIN) == 1) {} //tunggu low
    int64_t start = esp_timer_get_time();
    while (gpio_get_level(DHT_PIN) == 0) {} //tunggu high
    while (gpio_get_level(DHT_PIN) == 1) {} //tunggu low selesai

    //baca data 40 bit
    for (int i=0; i<40; i++) {
        // tunggu leading low 50us
        while(gpio_get_level(DHT_PIN) == 0) {}

        //ukur durasi high
        start = esp_timer_get_time();
        while (gpio_get_level(DHT_PIN) == 1) {}
        int duration = esp_timer_get_time() - start;

        //decode bit
        data[i/8] <<=1;
        if (duration >50) data[i/8] |=1;
        
        }
    if (data[4]== (data[0] + data[1] + data[2] + data[3])) {
        printf("Humidity: %d.%d%%, Temperature: %d.%d C\n", data[0], data[1], data[2], data[3]);
    } else {
        printf("Checksum error! Receiverd: %d, %d, %d, %d, %d\n", data[0], data[1], data[2], data[3], data[4]);
    }

}

void app_main() {
    while (1) {
        read_dht();
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
