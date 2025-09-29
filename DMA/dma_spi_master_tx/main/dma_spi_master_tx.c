#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "esp_err.h"

#define SPI_HOST HSPI_HOST
#define DMA_CH 1
#define SPI_CLK_HZ 1000000

spi_device_handle_t spi;

void spi_dma_init() {
    spi_bus_config_t buscfg = {
        .mosi_io_num = 23,
        .miso_io_num = -1,
        .sclk_io_num = 18,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096, //max dma transfer size
    };

    ESP_ERROR_CHECK(spi_bus_initialize(SPI_HOST, &buscfg, DMA_CH));

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = SPI_CLK_HZ,
        .mode = 0,
        .spics_io_num = 5,
        .queue_size = 1,
        .flags = SPI_DEVICE_HALFDUPLEX,
    };

    ESP_ERROR_CHECK(spi_bus_add_device(SPI_HOST, &devcfg, &spi));

    printf("SPI DMA Initialized\n");
}

void spi_dma_send(uint8_t *data, size_t len) {
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));

    t.length = len * 8; //bit
    t.tx_buffer = data;
    t.rx_buffer = NULL;

    ESP_ERROR_CHECK(spi_device_transmit(spi, &t));
    printf("Sent %d bytes via DMA\n", len);
}

void app_main(void)
{
    uint8_t buffer[1024];
    spi_dma_init();
    for (int i =0; i < sizeof(buffer); i++) {
        buffer[i] = i & 0xFF;
    }
    while (1) {
        spi_dma_send(buffer, sizeof(buffer));
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
