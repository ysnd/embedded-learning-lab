#ifndef FLAME_SENSOR_H
#define  FLAME_SENSOR_H

#include "driver/gpio.h"

typedef struct {
    gpio_num_t pin;
} flame_sensor_t;

void flame_sensor_int(flame_sensor_t *sensor, gpio_num_t pin);

bool flame_sensor_is_detected(flame_sensor_t *sensor);

#endif
