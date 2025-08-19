#pragma once
#include <stdint.h>

typedef struct {
    int pin;
} ds18b20_t;


/**
 * @brief Inisialisasi pin 1-wire
 * @param pin Nomor GPIO
 */
void ds18b20_init(int8_t pin);

/**
 * @brief Baca suhu dalam satuan Celcius
 * @return suhu C 999.0 if sensor error
 */
float ds18b20_read_temp(int pin);
