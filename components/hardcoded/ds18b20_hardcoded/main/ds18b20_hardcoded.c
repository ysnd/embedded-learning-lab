#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"


#define ow_pin 4

static uint8_t ds18b20_reset(void) {
    gpio_set_direction(ow_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(ow_pin, 0);
    esp_rom_delay_us(480); //reset pulse 480us 
    gpio_set_level(ow_pin, 1);
    esp_rom_delay_us(70); //tunggu presence
    gpio_set_direction(ow_pin, GPIO_MODE_INPUT);
    uint8_t presence = gpio_get_level(ow_pin) == 0 ? 1 : 0;
    esp_rom_delay_us(410); //finish timeslot
    return presence;
}

static void ds18b20_write_bit(uint8_t bit) {
    gpio_set_direction(ow_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(ow_pin, 0);
    if (bit) {
        esp_rom_delay_us(6); //tulis 1
        gpio_set_level(ow_pin, 1);
        esp_rom_delay_us(64); //reset slot
    } else {
        esp_rom_delay_us(60); //tulis 0
        gpio_set_level(ow_pin, 1);
        esp_rom_delay_us(10); //recovery
    }
}

static uint8_t ds18b20_read_bit() {
    uint8_t bit;
    gpio_set_direction(ow_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(ow_pin, 0);
    esp_rom_delay_us(6);
    gpio_set_direction(ow_pin, GPIO_MODE_INPUT);
    esp_rom_delay_us(9);
    bit = gpio_get_level(ow_pin);
    esp_rom_delay_us(55);
    return bit;
}

static void ds18b20_write_byte(uint8_t data) {
    for (int i = 0; i < 8; i++) {
        ds18b20_write_bit((data >> i) & 0x01);
    }
}

static uint8_t ds18b20_read_byte(void) {
    uint8_t data = 0;
    for (int i = 0; i < 8; i++) {
        data |=ds18b20_read_bit() << i;
    }
    return data;
}

static float ds18b20_read_temp(void) {
    uint8_t lsb, msb;
    int16_t raw;
    if (!ds18b20_reset()) {
        printf("Sensor DS18B20 tidak terdeteksi!\n");
        return -127.0; // nilai error
    }
    ds18b20_write_byte(0xCC); // skip rom
    ds18b20_write_byte(0x44); // convert T
    vTaskDelay(pdMS_TO_TICKS(750)); // tunggu konversi 12bit

    if (!ds18b20_reset()) {
        return -127.0;
    }
    ds18b20_write_byte(0xCC); // skip rom
    ds18b20_write_byte(0xBE); // read scratchpad

    lsb = ds18b20_read_byte();
    msb = ds18b20_read_byte();

    raw = (msb <<8) | lsb;
    return (float)raw/16.0; //resolusi 0.0625 C/bit
}

void app_main(void){

    
    while (1) {
        float temp = ds18b20_read_temp();
        if (temp !=-127.0) {
            printf("Temperatur : %.2f C\n", temp);
        } else {
            printf("Gagal membaca sensor.\n");
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
