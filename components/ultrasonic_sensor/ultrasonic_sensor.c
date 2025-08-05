#include "ultrasonic_sensor.h"
#include "esp_timer.h"
#include "esp_rom_sys.h"

static ultrasonic_sensor_t ultrasonic_single;

void ultrasonic_sensor_init(gpio_num_t trig_pin, gpio_num_t echo_pin) {
    ultrasonic_multiple_sensor_init(&ultrasonic_single, trig_pin, echo_pin);
}

float ultrasonic_sensor_read_cm(void) {
    return ultrasonic_multiple_read_cm(&ultrasonic_single);
}

void ultrasonic_multiple_sensor_init(ultrasonic_sensor_t *sensor, gpio_num_t trig, gpio_num_t echo) {
    sensor->trig_pin = trig;
    sensor->echo_pin = echo;

    gpio_reset_pin(trig);
    gpio_reset_pin(echo);

    gpio_set_direction(trig, GPIO_MODE_OUTPUT);
    gpio_set_direction(echo, GPIO_MODE_INPUT);

    gpio_set_level(trig, 0);
}

float ultrasonic_multiple_read_cm(ultrasonic_sensor_t *sensor) {
    gpio_set_level(sensor->trig_pin, 0);
    esp_rom_delay_us(2);
    gpio_set_level(sensor->trig_pin, 1);
    esp_rom_delay_us(10);
    gpio_set_level(sensor->trig_pin, 0);

    int64_t start = esp_timer_get_time();
    while (gpio_get_level(sensor->echo_pin) == 0) {
        if ((esp_timer_get_time() - start) > 100000) return -1;
    }

    int64_t echo_start = esp_timer_get_time();
    while (gpio_get_level(sensor->echo_pin) == 1) {
        if ((esp_timer_get_time() - echo_start) > 100000) return -1;        
        }
    int64_t echo_end = esp_timer_get_time();
    int64_t duration = echo_end - echo_start;

    return duration * 0.0343 /2.0;
}
