#include "ds18b20.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "include/ds18b20.h"

static uint8_t ds18b20_reset(int pin) {
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);
    esp_rom_delay_us(480); //reset pulse 480us 
    gpio_set_level(pin, 1);
    esp_rom_delay_us(70); //tunggu presence
    gpio_set_direction(pin, GPIO_MODE_INPUT);
    uint8_t presence = gpio_get_level(pin) == 0 ? 1 : 0;
    esp_rom_delay_us(410); //finish timeslot
    return presence;
}

static void ds18b20_write_bit(int pin, uint8_t bit) {
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);
    if (bit) {
        esp_rom_delay_us(6); //tulis 1
        gpio_set_level(pin, 1);
        esp_rom_delay_us(64); //reset slot
    } else {
        esp_rom_delay_us(60); //tulis 0
        gpio_set_level(pin, 1);
        esp_rom_delay_us(10); //recovery
    }
}

static uint8_t ds18b20_read_bit(int pin) {
    uint8_t bit;
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);
    esp_rom_delay_us(6);
    gpio_set_direction(pin, GPIO_MODE_INPUT);
    esp_rom_delay_us(9);
    bit = gpio_get_level(pin);
    esp_rom_delay_us(55);
    return bit;
}

static void ds18b20_write_byte(int pin, uint8_t data) {
    for (int i = 0; i < 8; i++) {
        ds18b20_write_bit(pin, (data >> i) & 0x01);
    }
}

static uint8_t ds18b20_read_byte(int pin) {
    uint8_t data = 0;
    for (int i = 0; i < 8; i++) {
        data |=ds18b20_read_bit(pin) << i;
    }
    return data;
}

static float ds18b20_read_temp_raw(int pin) {
    uint8_t lsb, msb;
    int16_t raw;
    if (!ds18b20_reset(pin)) {
        printf("Sensor DS18B20 tidak terdeteksi!\n");
        return -127.0; // nilai error
    }
    ds18b20_write_byte(pin, 0xCC); // skip rom
    ds18b20_write_byte(pin, 0x44); // convert T
    vTaskDelay(pdMS_TO_TICKS(750)); // tunggu konversi 12bit

    if (!ds18b20_reset(pin)) {
        return -127.0;
    }
    ds18b20_write_byte(pin, 0xCC); // skip rom
    ds18b20_write_byte(pin, 0xBE); // read scratchpad

    lsb = ds18b20_read_byte(pin);
    msb = ds18b20_read_byte(pin);

    raw = (msb <<8) | lsb;
    return (float)raw/16.0; //resolusi 0.0625 C/bit
}

void ds18b20_init(int8_t pin) {
    gpio_config_t io = {
        .pin_bit_mask = 1ULL << pin,
        .mode = GPIO_MODE_INPUT_OUTPUT_OD,
        .pull_up_en = 1,
        .pull_down_en = 0,
        .intr_type = 0
    };
    gpio_config(&io);
}

float ds18b20_read_temp(int pin) {
    return ds18b20_read_temp_raw(pin);
}
