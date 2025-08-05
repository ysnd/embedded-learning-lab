#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#include "driver/gpio.h"

/**
 * Struct for pin configuration sensor ultrasonic (multiple sensor)
 * */
typedef struct {
    gpio_num_t trig_pin;
    gpio_num_t echo_pin;
} ultrasonic_sensor_t;

/**
 * Initialization ultrasonic sensor for single mode
 * @param trig_pin output trigger pin
 * @param echo_pin input echo pin
 */
void ultrasonic_sensor_init(gpio_num_t trig_pin, gpio_num_t echo_pin);

/** Read distance from ultrasonic sensor (single mode)
 * @return distance in cm, or -1 if timeout
 */

float ultrasonic_sensor_read_cm(void);

/**
 * Initialization ultrasonic (multiple sensors)
 * @param Pointer sensor to struct sensor
 * @param trig_pin trigger pin
 * @param echo_pin echo pin
 */

void ultrasonic_multiple_sensor_init(ultrasonic_sensor_t *sensor, gpio_num_t trig_pin, gpio_num_t echo_pin);

/**
 * Read distance from multiple ultrasonic sensors
 * @param Pointer sensor to sensor who Initialization
 * @return distance in cm, or -1 if timeout
 */

float ultrasonic_multiple_read_cm(ultrasonic_sensor_t *sensor);

#endif
