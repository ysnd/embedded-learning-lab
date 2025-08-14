#pragma once
#include <stdbool.h>
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float humidity;
    float temperature;
} dht11_data_t;

/**
 * @brief Initialization pin DHT11
 * @param gpio_num GPIO number
 */
void dht11_init(gpio_num_t gpio_num);

/**
 * @brief Read sensor data
 * @param data pointer to struct data
 * @return true if succes, false if failed
 */
bool dht11_read(dht11_data_t *data);

#ifdef  __cplusplus
}
#endif
