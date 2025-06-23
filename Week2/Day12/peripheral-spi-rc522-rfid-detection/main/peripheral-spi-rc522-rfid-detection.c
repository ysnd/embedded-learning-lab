// rc522_full_driver.c - ESP-IDF RC522 full low-level driver
#include <stdio.h>
#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_rom_sys.h"

#define PIN_MISO 19
#define PIN_MOSI 23
#define PIN_CLK 18
#define PIN_CS 5

#define RC522_TAG "RC522"

spi_device_handle_t rc522;

void rc522_spi_init() {
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_MISO,
        .mosi_io_num = PIN_MOSI,
        .sclk_io_num = PIN_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 0,
    };
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1 * 1000 * 1000,
        .mode = 0,
        .spics_io_num = PIN_CS,
        .queue_size = 7,
    };

    ESP_ERROR_CHECK(spi_bus_initialize(SPI3_HOST, &buscfg, SPI_DMA_CH_AUTO));
    ESP_ERROR_CHECK(spi_bus_add_device(SPI3_HOST, &devcfg, &rc522));
}

uint8_t rc522_read_register(uint8_t reg) {
    uint8_t tx_buf[2] = { ((reg << 1) & 0x7E) | 0x80, 0x00 };
    uint8_t rx_buf[2] = { 0 };
    spi_transaction_t trans = {
        .length = 8 * 2,
        .tx_buffer = tx_buf,
        .rx_buffer = rx_buf,
    };
    ESP_ERROR_CHECK(spi_device_transmit(rc522, &trans));
    return rx_buf[1];
}

void rc522_write_register(uint8_t reg, uint8_t value) {
    uint8_t tx_buf[2] = { ((reg << 1) & 0x7E), value };
    spi_transaction_t trans = {
        .length = 8 * 2,
        .tx_buffer = tx_buf,
        .rx_buffer = NULL,
    };
    ESP_ERROR_CHECK(spi_device_transmit(rc522, &trans));
}

void rc522_reset() {
    rc522_write_register(0x01, 0x0F);
    esp_rom_delay_us(5000);
}

void rc522_antena_on() {
    rc522_write_register(0x14, 0x03);
}

void rc522_clear_bitmask(uint8_t reg, uint8_t mask) {
    uint8_t tmp = rc522_read_register(reg);
    rc522_write_register(reg, tmp & (~mask));
}

void rc522_set_bitmask(uint8_t reg, uint8_t mask) {
    uint8_t tmp = rc522_read_register(reg);
    rc522_write_register(reg, tmp | mask);
}

bool rc522_request() {
    rc522_write_register(0x0D, 0x07);
    rc522_clear_bitmask(0x04, 0x80);
    rc522_write_register(0x01, 0x00);
    rc522_write_register(0x0A, 0x80);
    rc522_write_register(0x09, 0x26);
    rc522_write_register(0x01, 0x0C);
    rc522_write_register(0x0D, 0x87);

    int timeout = 25;
    while (timeout--) {
        uint8_t irq = rc522_read_register(0x04);
        if (irq & 0x30) break;
        esp_rom_delay_us(100);
    }

    uint8_t error = rc522_read_register(0x06);
    if (error & 0x1B) return false;
    uint8_t fifo_level = rc522_read_register(0x0A);
    return fifo_level != 0;
}

bool rc522_anticollision(uint8_t *uid) {
    rc522_write_register(0x0D, 0x00);
    rc522_clear_bitmask(0x04, 0x80);
    rc522_write_register(0x01, 0x00);
    rc522_write_register(0x0A, 0x80);

    rc522_write_register(0x09, 0x93); // Anti-collision CL1
    rc522_write_register(0x09, 0x20);

    rc522_write_register(0x01, 0x0C);
    rc522_write_register(0x0D, 0x87);

    int timeout = 25;
    while (timeout--) {
        uint8_t irq = rc522_read_register(0x04);
        if (irq & 0x30) break;
        esp_rom_delay_us(100);
    }

    uint8_t error = rc522_read_register(0x06);
    if (error & 0x1B) return false;

    uint8_t len = rc522_read_register(0x0A);
    if (len < 5) return false;

    for (int i = 0; i < 5; i++) {
        uid[i] = rc522_read_register(0x09);
    }
    return true;
}

void app_main(void) {
    rc522_spi_init();
    rc522_reset();
    rc522_antena_on();
    vTaskDelay(pdMS_TO_TICKS(50));

    uint8_t version = rc522_read_register(0x37);
    ESP_LOGI(RC522_TAG, "VersionReg : 0x%02X", version);

    while (1) {
        if (rc522_request()) {
            uint8_t uid[5];
            if (rc522_anticollision(uid)) {
                ESP_LOGI(RC522_TAG, "UID: %02X %02X %02X %02X %02X", uid[0], uid[1], uid[2], uid[3], uid[4]);
            } else {
                ESP_LOGW(RC522_TAG, "Gagal anticollision");
            }
        } else {
            ESP_LOGI(RC522_TAG, "Menunggu kartu...");
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
