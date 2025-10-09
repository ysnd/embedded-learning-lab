#include <stdbool.h>
#include <stdio.h>
#include "esp_err.h"
#include "esp_spiffs.h"


void app_main(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = "spiffs",
        .max_files = 5,
        .format_if_mount_failed = true
    };

    //mount spiffs
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        printf("SPIFFS mount failed\n");
        return;
    }
    // info spiffs
    size_t total = 0, used = 0;
    esp_spiffs_info(conf.partition_label, &total, &used);
    printf("Total: %zu, Used: %zu, Free:%zu\n", total, used, total - used);

    //buat beberapa file dummy
    for (int i=1; i<=5; i++) {
        char filename[32];
        snprintf(filename, sizeof(filename), "/spiffs/file%d.txt", i);
        FILE *f = fopen(filename, "w");
        if (f == NULL) {
            printf("Failed to create %s\n", filename);
            continue;
        }
        fprintf(f, "This is file %d in SPIFFS\n", i);
        fclose(f);
        printf("Created %s\n", filename);
    }
    //info
    esp_spiffs_info(conf.partition_label, &total, &used);
    printf("After creating files - Total:%zu, used: %zu, Free: %zu\n", total, used, total - used);

    //delete file dummy
    for (int i=1; i<=5; i++) {
        char filename[32];
        snprintf(filename, sizeof(filename), "/spiffs/file%d.txt", i);
        if (remove(filename) == 0) {
            printf("Delete %s\n", filename);
        } else {
            printf("Failed to delete %s\n",filename);
        }
    }
    //end info
    esp_spiffs_info(conf.partition_label, &total, &used);
    printf("After deleting files - Total: %zu, Used: %zu, Free: %zu\n",total, used, total - used);
    //unmount spiffs
    esp_vfs_spiffs_unregister(conf.partition_label);
    printf("SPIFFS unmounted");
}
