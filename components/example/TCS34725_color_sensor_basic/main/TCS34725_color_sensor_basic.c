#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "TCS34725.h"


void app_main(void)
{
    tcs34725_conf_t tcs = {
        .i2c_port = I2C_NUM_0,
        .sda_pin = 21,
        .scl_pin = 22,
        .freq_hz = 100000
    };
    tcs34725_init(&tcs);
    

    while (1) {
        uint16_t c, r, g, b;
        if (tcs34725_data_ready()) {
            tcs34725_read_rgbc(&c, &r, &g, &b);
            printf("C : %u | R : %u | G : %u | B : %u\n", c, r, g, b);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
