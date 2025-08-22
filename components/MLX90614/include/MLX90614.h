#pragma once
#include <stdint.h>

#define MLX_ADDR 0x5A
#define MLX_REG_AMB 0x06
#define MLX_REG_OBJ 0x07

typedef struct {
    uint8_t i2c_port;
    uint8_t sda_pin;
    uint8_t scl_pin;
    int freq_hz;
} mlx_conf_t;

void mlx_init(mlx_conf_t *cfg);
float mlx_read_temperature_ambient();
float mlx_read_temperature_object();


