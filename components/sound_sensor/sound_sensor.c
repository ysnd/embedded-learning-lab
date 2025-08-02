#include "sound_sensor.h"
#include "include/sound_sensor.h"

void sound_sensor_init(sound_sensor_t *sensor, gpio_num_t pin) {
    sensor->pin = pin;

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << pin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
}

bool sound_sensor_is_detected(sound_sensor_t *sensor) {
    return gpio_get_level(sensor->pin) == 0;
}
