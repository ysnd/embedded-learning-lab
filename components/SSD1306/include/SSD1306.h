#pragma once

#include <stdint.h>
#include <stdbool.h>

#define SSD1306_ADDR 0x3C

typedef struct {
    uint8_t i2c_port;
    uint8_t sda_pin;
    uint8_t scl_pin;
    int freq_hz;
} ssd1306_conf_t;

void ssd1306_init(ssd1306_conf_t *conf);
void ssd1306_clear(void);
void ssd1306_update_display(void);
void ssd1306_draw_string(int x, int y, const char* str, bool invert);
void ssd1306_draw_rect(int x, int y, int w, int h, bool color);
void ssd1306_fill_rect(int x, int y, int w, int h, bool color);
void ssd1306_draw_circle(int x0, int y0, int r, bool color);
void ssd1306_fill_circle(int x0, int y0, int r, bool color);
void ssd1306_draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, bool color);
void ssd1306_fill_triangle(int x0, int y0, int x1, int y1, int x2, int y2, bool color);
void ssd1306_scroll_right(uint8_t start_page, uint8_t end_page, uint8_t frame_interval);
void ssd1306_scroll_left(uint8_t start_page, uint8_t end_page, uint8_t frame_interval);
void ssd1306_scroll_diag_right( uint8_t start_page, uint8_t end_page, uint8_t frame_interval, uint8_t offset);
void ssd1306_scroll_diag_left( uint8_t start_page, uint8_t end_page, uint8_t frame_interval, uint8_t offset);
void ssd1306_stop_scroll(void);
