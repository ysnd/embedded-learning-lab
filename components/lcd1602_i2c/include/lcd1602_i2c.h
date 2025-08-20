#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t addr;
    uint8_t sda_pin;
    uint8_t scl_pin;
    int freq_hz;
} lcd_conf_t;

void lcd_init(const lcd_conf_t *cfg);
void lcd_clear(void);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_print(const char *str);
