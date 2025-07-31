#ifndef PIR_SENSOR_H
#define PIR_SENSOR_H

#include "driver/gpio.h"

typedef struct {
    gpio_num_t pin;
} pir_sensor_t;

void pir_sensor_init(pir_sensor_t *sensor, gpio_num_t pin);
bool pir_sensor_is_motion_detected(pir_sensor_t *sensor);

#endif // PIR_SENSOR_H

