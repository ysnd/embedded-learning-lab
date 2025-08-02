#ifndef SOUND_SENSOR_H
#define SOUND_SENSOR_H

#include "driver/gpio.h"

typedef struct {
    gpio_num_t pin;
} sound_sensor_t;

void sound_sensor_init(sound_sensor_t *sensor, gpio_num_t pin);
bool sound_sensor_is_detected(sound_sensor_t *sensor);

#endif
