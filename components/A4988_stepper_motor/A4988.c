#include "A4988.h"
#include "esp_rom_sys.h"
#include <stdint.h>

static stepper_config_t pin;

static const uint8_t tbl[5][3] = {
    {0,0,0}, {1,0,0}, {0,1,0}, {1,1,0}, {1,1,1}
};

void stepper_init(const stepper_config_t *p) {
    pin = *p;
    gpio_config_t io = {
        .pin_bit_mask = (1ULL<<pin.step) | (1ULL<<pin.dir) | (1ULL<<pin.en) | (1ULL<<pin.ms1) | (1ULL<<pin.ms2) | (1ULL<<pin.ms3),
        .mode = GPIO_MODE_OUTPUT
    };
    gpio_config(&io);
    gpio_set_level(pin.en, 0);
    stepper_set_mode(FULL_STEP_MODE);
}

void stepper_set_mode(stepper_mode_t mode) {
    if (mode>SIXTEENTH_STEP_MODE) return;
    gpio_set_level(pin.ms1, tbl[mode][0]);
    gpio_set_level(pin.ms2, tbl[mode][1]);
    gpio_set_level(pin.ms3, tbl[mode][2]);
}

void stepper_move(int32_t steps, uint32_t pulse_us, stepper_mode_t mode) {
    stepper_set_mode(mode);
    if (steps>0) {
        gpio_set_level(pin.dir, 1);
    } else {
        gpio_set_level(pin.dir, 0);
    }
    if (steps<0) {
        steps=-steps;
    }
    for (int32_t i=0; i<steps; ++i) {
        gpio_set_level(pin.step, 1);
        esp_rom_delay_us(pulse_us);
        gpio_set_level(pin.step, 0);
        esp_rom_delay_us(pulse_us);
    }
}

