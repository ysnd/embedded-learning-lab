#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "MLX90614.h"


void app_main(void)
{
    mlx_conf_t mlx = {
        .i2c_port = I2C_NUM_0,
        .sda_pin = 21,
        .scl_pin = 22,
        .freq_hz = 100000
    };
    mlx_init(&mlx);

    while (1) {
        float to = mlx_read_temperature_object();
        float ta = mlx_read_temperature_ambient();
        printf("Object : %.2f C | Ambient : %.2f C\n", to, ta);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
