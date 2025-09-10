#include <stdio.h>
#include "RC522_RFID.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

void app_main(void)
{
    rc522_conf_t conf = {
        .miso_pin = 19,
        .mosi_pin = 23,
        .sck_pin  = 18,
        .cs_pin   = 5,
        .rst_pin  = 21,
        .clock_speed_hz = 1000000
    };
    rc522_init(&conf);
    while (1) {
        uint8_t uid[10];    // buffer untuk UID
        uint8_t uid_len = 0; // panjang UID

        uint8_t status = detect_card(uid, &uid_len);

        if (status == 0) {
            printf("Kartu terdeteksi! UID (%d byte): ", uid_len);
            for (int i = 0; i < uid_len; i++) {
                printf("%02X ", uid[i]);
            }
            printf("\n");
        } else if (status == 1) {
            printf("Tidak ada kartu terdeteksi.\n");
        } else {
            printf("Error membaca kartu! Status: %d\n", status);
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS); // delay 1 detik
    }
}
