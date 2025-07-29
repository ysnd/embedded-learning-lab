#ifndef IR_OBSTACLE_H
#define IR_OBSTACLE_H

#include "driver/gpio.h"

typedef struct {
    gpio_num_t pin;
} ir_obstacle_t;

void ir_obstacle_init(ir_obstacle_t *sensor, gpio_num_t pin);
bool ir_obstacle_read(ir_obstacle_t *sensor);

#endif
