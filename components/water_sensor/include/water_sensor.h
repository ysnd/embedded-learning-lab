#ifndef WATER_SENSOR_H
#define WATER_SENSOR_H

#include "driver/gpio.h"

typedef struct {
    gpio_num_t pin;
} water_sensor_t;

void water_sensor_init(water_sensor_t *sensor, gpio_num_t pin);
bool water_sensor_is_detected(water_sensor_t *sensor);

#endif
