#include "esp_err.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "sd_protocol_types.h"
#include "sdmmc_cmd.h"
#include <stdbool.h>
#include <stdio.h>

void app_main(void) {
    esp_err_t ret;
    printf("Initializing SD card (dummy test)\n");
    //config host   
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.flags = SDMMC_HOST_FLAG_1BIT;

    sdmmc_slot_config_t slot_cfg = SDMMC_SLOT_CONFIG_DEFAULT();

    //mount FATFS
    esp_vfs_fat_sdmmc_mount_config_t mount_cfg = {
        .format_if_mount_failed = false,
        .max_files = 3,
        .allocation_unit_size = 16 * 1024

    };

    sdmmc_card_t *card;
    ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_cfg, &mount_cfg, &card);

    if (ret !=ESP_OK) {
        printf("Card mount failed, err: %s\n", esp_err_to_name(ret));
    } else {
        printf("Card Mounted succesfully!\n");
        sdmmc_card_print_info(stdout, card);
    }

    FILE *f = fopen("/sdcard/helo.txt", "W");
    if (f == NULL) {
        printf("File open failed (expected withoout dvce)\n");
    } else {
        fprintf(f, "Helo SDMMC DMA test!\n");
        fclose(f);
        printf("Write done!\n");
    }
}
