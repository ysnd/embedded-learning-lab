#include <string.h>
#include "SSD1306.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static ssd1306_conf_t ssd1306_conf;

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64

static uint8_t display_buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

static const uint8_t font_5x7[][5] = {
	{0x00, 0x00, 0x00, 0x00, 0x00}, // spasi (32)
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // ! (33)
    {0x00, 0x07, 0x00, 0x07, 0x00}, // " (34)
    {0x14, 0x7F, 0x14, 0x7F, 0x14}, // # (35)
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // $ (36)
    {0x23, 0x13, 0x08, 0x64, 0x62}, // % (37)
    {0x36, 0x49, 0x55, 0x22, 0x50}, // & (38)
    {0x00, 0x05, 0x03, 0x00, 0x00}, // ' (39)
    {0x00, 0x1C, 0x22, 0x41, 0x00}, // ( (40)
    {0x00, 0x41, 0x22, 0x1C, 0x00}, // ) (41)
    {0x14, 0x08, 0x3E, 0x08, 0x14}, // * (42)
    {0x08, 0x08, 0x3E, 0x08, 0x08}, // + (43)
    {0x00, 0x50, 0x30, 0x00, 0x00}, // , (44)
    {0x08, 0x08, 0x08, 0x08, 0x08}, // - (45)
    {0x00, 0x60, 0x60, 0x00, 0x00}, // . (46)
    {0x20, 0x10, 0x08, 0x04, 0x02}, // / (47)
    
    // Angka 0-9 (48-57)
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // 9
    
    {0x00, 0x36, 0x36, 0x00, 0x00}, // : (58)
    {0x00, 0x56, 0x36, 0x00, 0x00}, // ; (59)
    {0x08, 0x14, 0x22, 0x41, 0x00}, // < (60)
    {0x14, 0x14, 0x14, 0x14, 0x14}, // = (61)
    {0x00, 0x41, 0x22, 0x14, 0x08}, // > (62)
    {0x02, 0x01, 0x51, 0x09, 0x06}, // ? (63)
    {0x32, 0x49, 0x79, 0x41, 0x3E}, // @ (64)
    
    // Huruf A-Z (65-90)
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
    {0x7F, 0x09, 0x09, 0x09, 0x01}, // F
    {0x3E, 0x41, 0x49, 0x49, 0x7A}, // G
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // I
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // J
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // K
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // L
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // S
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
    {0x3F, 0x40, 0x38, 0x40, 0x3F}, // W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X
    {0x07, 0x08, 0x70, 0x08, 0x07}, // Y
    {0x61, 0x51, 0x49, 0x45, 0x43}, // Z
    
    {0x00, 0x7F, 0x41, 0x41, 0x00}, // [ (91)
    {0x02, 0x04, 0x08, 0x10, 0x20}, // \ (92)
    {0x00, 0x41, 0x41, 0x7F, 0x00}, // ] (93)
    {0x04, 0x02, 0x01, 0x02, 0x04}, // ^ (94)
    {0x40, 0x40, 0x40, 0x40, 0x40}, // _ (95)
    {0x00, 0x01, 0x02, 0x04, 0x00}, // ` (96)
    
    // Huruf a-z (97-122)
    {0x20, 0x54, 0x54, 0x54, 0x78}, // a
    {0x7F, 0x48, 0x44, 0x44, 0x38}, // b
    {0x38, 0x44, 0x44, 0x44, 0x20}, // c
    {0x38, 0x44, 0x44, 0x48, 0x7F}, // d
    {0x38, 0x54, 0x54, 0x54, 0x18}, // e
    {0x08, 0x7E, 0x09, 0x01, 0x02}, // f
    {0x0C, 0x52, 0x52, 0x52, 0x3E}, // g
    {0x7F, 0x08, 0x04, 0x04, 0x78}, // h
    {0x00, 0x44, 0x7D, 0x40, 0x00}, // i
    {0x20, 0x40, 0x44, 0x3D, 0x00}, // j
    {0x7F, 0x10, 0x28, 0x44, 0x00}, // k
    {0x00, 0x41, 0x7F, 0x40, 0x00}, // l
    {0x7C, 0x04, 0x18, 0x04, 0x78}, // m
    {0x7C, 0x08, 0x04, 0x04, 0x78}, // n
    {0x38, 0x44, 0x44, 0x44, 0x38}, // o
    {0x7C, 0x14, 0x14, 0x14, 0x08}, // p
    {0x08, 0x14, 0x14, 0x18, 0x7C}, // q
    {0x7C, 0x08, 0x04, 0x04, 0x08}, // r
    {0x48, 0x54, 0x54, 0x54, 0x20}, // s
    {0x04, 0x3F, 0x44, 0x40, 0x20}, // t
    {0x3C, 0x40, 0x40, 0x20, 0x7C}, // u
    {0x1C, 0x20, 0x40, 0x20, 0x1C}, // v
    {0x3C, 0x40, 0x30, 0x40, 0x3C}, // w
    {0x44, 0x28, 0x10, 0x28, 0x44}, // x
    {0x0C, 0x50, 0x50, 0x50, 0x3C}, // y
    {0x44, 0x64, 0x54, 0x4C, 0x44}, // z
    
    {0x00, 0x08, 0x36, 0x41, 0x00}, // { (123)
    {0x00, 0x00, 0x7F, 0x00, 0x00}, // | (124)
    {0x00, 0x41, 0x36, 0x08, 0x00}, // } (125)
    {0x08, 0x08, 0x2A, 0x1C, 0x08}, // ~ (126)
    {0x08, 0x1C, 0x2A, 0x08, 0x08}  // DEL (127)
};

static esp_err_t i2c_master_init(ssd1306_conf_t *conf) {
    i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = conf->sda_pin,
        .scl_io_num = conf->scl_pin,
        .master.clk_speed = conf->freq_hz
    };
    esp_err_t err = i2c_param_config(conf->i2c_port, &i2c_conf);
    if (err != ESP_OK) return err;
        return i2c_driver_install(conf->i2c_port, I2C_MODE_MASTER, 0, 0, 0);
}

static esp_err_t ssd1306_command(uint8_t command) {
    uint8_t buf[2] = {0x00, command};

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SSD1306_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd, buf, sizeof(buf), true);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(ssd1306_conf.i2c_port, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
    return ret;
}

static esp_err_t ssd1306_write_data(uint8_t *data, size_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SSD1306_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x40, true);
    i2c_master_write(cmd, data, len, true);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(ssd1306_conf.i2c_port, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
    return ret;
}

void ssd1306_init(ssd1306_conf_t *conf) {
    ssd1306_conf = *conf;
    i2c_master_init(conf);

    vTaskDelay(pdMS_TO_TICKS(100));
    ssd1306_command(0xAE); // Display OFF
    ssd1306_command(0xD5); // Set display clock divide ratio/oscillator frequency
    ssd1306_command(0x80); // Set divide ratio
    ssd1306_command(0xA8); // Set multiplex ratio
    ssd1306_command(0x3F); // 1/64 duty
    ssd1306_command(0xD3); // Set display offset
    ssd1306_command(0x00); // No offset
    ssd1306_command(0x40); // Set start line address
    ssd1306_command(0x8D); // Charge pump setting
    ssd1306_command(0x14); // Enable charge pump
    ssd1306_command(0x20); // Set memory addressing mode
    ssd1306_command(0x00); // Horizontal addressing mode
    ssd1306_command(0xA1); // Set segment re-map
    ssd1306_command(0xC8); // Set COM output scan direction
    ssd1306_command(0xDA); // Set COM pins hardware configuration
    ssd1306_command(0x12); // Alternative COM pin configuration
    ssd1306_command(0x81); // Set contrast control
    ssd1306_command(0xCF); // Set contrast value
    ssd1306_command(0xD9); // Set pre-charge period
    ssd1306_command(0xF1); // Set pre-charge value
    ssd1306_command(0xDB); // Set VCOMH deselect level
    ssd1306_command(0x40); // Set VCOMH value
    ssd1306_command(0xA4); // Entire display ON (resume)
    ssd1306_command(0xA6); // Normal display mode
    ssd1306_command(0xAF); // Display ON
}

void ssd1306_clear(void) {
    for(int i=0; i<SSD1306_WIDTH*SSD1306_HEIGHT/8; i++)
        display_buffer[i] = 0x00;
}

void ssd1306_update_display(void) {
    for (int page=0; page<8; page++) {
        ssd1306_command(0xB0 + page);
        ssd1306_command(0x00);
        ssd1306_command(0x10);
        ssd1306_write_data(&display_buffer[page*SSD1306_WIDTH], SSD1306_WIDTH);
    }
}


void ssd1306_draw_char(int x, int y, char c, bool invert) {
    if (c < 32 || c > 127) return;
    int index = c - 32;

    // Loop 6 kolom: 5 kolom font + 1 kolom spasi
    for (int i = 0; i < 6; i++) {
        uint8_t col = (i < 5) ? font_5x7[index][i] : 0x00; // kolom font atau spasi

        for (int j = 0; j < 8; j++) {
            bool pixel_on = col & (1 << j);
            int page = (y + j) / 8;
            int bit = (y + j) % 8;
            int pos = page * SSD1306_WIDTH + (x + i);

            if (invert) {
                // background putih
                display_buffer[pos] |= (1 << bit);

                // tulis huruf jadi hitam
                if (pixel_on) {
                    display_buffer[pos] &= ~(1 << bit);
                }
            } else {
                if (pixel_on) {
                    display_buffer[pos] |= (1 << bit);
                }
            }
        }
    }
}

void ssd1306_draw_string(int x, int y, const char* str, bool invert) {
    int cursor = x;
    while (*str) {
        ssd1306_draw_char(cursor, y, *str, invert);
        cursor+=6;
        str++;
    }
}

void ssd1306_draw_pixel(int x, int y, bool color) {
    if (x < 0 || x>= SSD1306_WIDTH || y < 0 || y >= SSD1306_HEIGHT) return;
    
    int page = y / 8;
    int bit = y % 8;
    int pos = page * SSD1306_WIDTH + x;

    if (color) {
        display_buffer[pos] |= (1 << bit);
    } else {
        display_buffer[pos] &= ~(1 << bit);
    }
}

void ssd1306_draw_rect(int x, int y, int w, int h, bool color) {
    for (int i= x; i < x + w; i++) {
        ssd1306_draw_pixel(i, y, color);
        ssd1306_draw_pixel(i, y + h - 1, color);
    }
    for (int j = y; j < y +h; j++) {
        ssd1306_draw_pixel(x, j, color);
        ssd1306_draw_pixel(x + w - 1, j, color);
    }
}

void ssd1306_fill_rect(int x, int y, int w, int h, bool color) {
    for (int i = x; i < x + w; i++) {
        for (int j = y; j < y + h; j++) {
            ssd1306_draw_pixel(i, j, color);
        }
    }
}

void ssd1306_draw_circle(int x0, int y0, int r, bool color) {
    int x = r;
    int y = 0;
    int err = 0;

    while (x >= y) {
        ssd1306_draw_pixel(x0 + x, y0 + y, color);
        ssd1306_draw_pixel(x0 + y, y0 + x, color);
        ssd1306_draw_pixel(x0 - y, y0 + x, color);
        ssd1306_draw_pixel(x0 - x, y0 + y, color);
        ssd1306_draw_pixel(x0 - x, y0 - y, color);
        ssd1306_draw_pixel(x0 - y, y0 - x, color);
        ssd1306_draw_pixel(x0 + y, y0 - x, color);
        ssd1306_draw_pixel(x0 + x, y0 - y, color);

        y += 1;
        if (err <= 0) {
            err += 2*y + 1;
        } else {
            x -= 1;
            err += 2*(y - x) + 1;
        }
    }
}

void ssd1306_fill_circle(int x0, int y0, int r, bool color) {
    for (int y = -r; y <= r; y++) {
        for (int x = -r; x <= r; x++) {
            if (x*x + y*y <= r*r) {
                ssd1306_draw_pixel(x0 + x, y0 + y, color);
            }
        }
    }
}
