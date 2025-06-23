#include <stdio.h>
#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#define PIN_MISO 19
#define PIN_MOSI 23
#define PIN_CLK 18
#define PIN_CS 5

static const char *TAG = "RC522";
spi_device_handle_t rc522;

void rc522_spi_init() {
    spi_bus_config_t buscfg = {
        .miso_io_num    = PIN_MISO,
        .mosi_io_num    = PIN_MOSI,
        .sclk_io_num    = PIN_CLK,
        .quadwp_io_num  = -1,
        .quadhd_io_num  = -1,
        .max_transfer_sz    = 0,
    };
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1 * 1000 * 1000,
        .mode   = 0,
        .spics_io_num   = PIN_CS,
        .queue_size = 7,
    };

    ESP_ERROR_CHECK(spi_bus_initialize(SPI3_HOST, &buscfg, SPI_DMA_CH_AUTO));
    ESP_ERROR_CHECK(spi_bus_add_device(SPI3_HOST, &devcfg, &rc522));
}

uint8_t rc522_read_register(uint8_t reg) {
    uint8_t tx_buf[2];
    uint8_t rx_buf[2];

    tx_buf[0] = ((reg << 1) & 0x7E) | 0x80;
    tx_buf[1] = 0x00;

    spi_transaction_t trans = {
        .length = 8 * 2,
        .tx_buffer = tx_buf,
        .rx_buffer = rx_buf,
    };
    ESP_ERROR_CHECK(spi_device_transmit(rc522, &trans));

    return rx_buf[1];
}

void app_main(void)
{
    rc522_spi_init();
    vTaskDelay(pdMS_TO_TICKS(100));

    uint8_t version = rc522_read_register(0x37);
    ESP_LOGI(TAG, "VersionReg : 0x%02X", version);

    if (version == 0x91 || version == 0x92) {
        ESP_LOGI(TAG, "RC522 terdeteksi! Komunikasi SPI berhasil.");
    }
    else {
        ESP_LOGW(TAG, "Gagal baca RC522. Cek kabel dan pin SPI !");
    }
}
