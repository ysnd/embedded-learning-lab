#include "SSD1306.h"
#include "driver/i2c.h"

void app_main(void) {
    ssd1306_conf_t conf = {
        .i2c_port = I2C_NUM_0,
        .sda_pin = 21,
        .scl_pin = 22,
        .freq_hz = 100000
    };
    ssd1306_init(&conf);

     while (1) {
        ssd1306_clear();

        // Circle outline
        ssd1306_draw_circle(32, 32, 15, true);

        // Circle filled
        ssd1306_fill_circle(96, 32, 15, true);

        // Label text
        ssd1306_draw_string(10, 5, "Outline", false);
        ssd1306_draw_string(70, 5, "Fill", false);

        ssd1306_update_display();

        vTaskDelay(pdMS_TO_TICKS(2000));

        // Invert test
        ssd1306_clear();
        ssd1306_draw_circle(64, 32, 25, true);
        ssd1306_draw_string(30, 55, "Big Circle", false);
        ssd1306_update_display();

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
