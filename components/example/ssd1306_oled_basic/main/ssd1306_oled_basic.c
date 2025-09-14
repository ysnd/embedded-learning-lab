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
        // === Tes teks ===
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

        // === Tes shape ===
        ssd1306_clear();
        ssd1306_draw_rect(0, 0, 40, 20, true);
        ssd1306_fill_rect(50, 0, 40, 20, true);
        ssd1306_draw_circle(20, 40, 10, true);
        ssd1306_fill_circle(60, 40, 10, true);
        ssd1306_draw_triangle(90, 10, 120, 30, 100, 50, true);
        ssd1306_update_display();
        vTaskDelay(pdMS_TO_TICKS(2000));

        // === Tes scroll ===
        ssd1306_clear();
        ssd1306_draw_string(10, 20, "SCROLL DEMO", false);
        ssd1306_update_display();
        
        // Scroll kanan
        ssd1306_scroll_right(0, 7, 0x04);
        vTaskDelay(pdMS_TO_TICKS(3000));
        ssd1306_stop_scroll();

        // Scroll kiri
        ssd1306_scroll_left(0, 7, 7);
        vTaskDelay(pdMS_TO_TICKS(3000));
        ssd1306_stop_scroll();

        // Scroll diagonal kanan
        ssd1306_scroll_diag_right(0, 7, 0x04, 1);
        vTaskDelay(pdMS_TO_TICKS(3000));
        ssd1306_stop_scroll();

    }
}
