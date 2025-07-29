#include "ir_obstacle.h"
#include "include/ir_obstacle.h"

void ir_obstacle_init(ir_obstacle_t *sensor, gpio_num_t pin) {
    sensor->pin = pin;
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << pin,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 1,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
}

bool ir_obstacle_read(ir_obstacle_t *sensor) {
    return gpio_get_level(sensor->pin) == 0;
}
