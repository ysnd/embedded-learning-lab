#pragma once

#include <stdint.h>
#include <stdbool.h>

#define TCS_ADDR 0x29
#define REG_EN 0x80 | 0x00
#define REG_ATIME 0x81 | 0x00
#define REG_GAIN 0x8F | 0x00
#define REG_STATUS 0x93 | 0x00
#define REG_CDATAL 0x94 | 0xA0

typedef struct {
    uint8_t i2c_port;
    uint8_t sda_pin;
    uint8_t scl_pin;
    int freq_hz;
} tcs34725_conf_t;

void tcs34725_init(tcs34725_conf_t *conf);
bool tcs34725_data_ready(void);
void tcs34725_read_rgbc(uint16_t *c, uint16_t *r, uint16_t *g, uint16_t *b);
