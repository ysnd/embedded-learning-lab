#include "esp_err.h"
#include <stdio.h>
#include "esp_spiffs.h"

void app_main(void)
{
    //1. config SPIFFS  
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            printf("Failed to mount or format filesystem\n");
        } else {
            printf("SPIFFS partition not found\n");
        }
        return;
    }
    //write files   
    FILE *f = fopen("/spiffs/hello.txt", "w");
    if (f == NULL) {
        printf("Failed to open file for writing\n");
        return;
    }
    fprintf(f, "Hello ESP32 SPIFFS!\n");
    fclose(f);
    printf("File Written\n");

    //read file 
    f = fopen("/spiffs/hello.txt", "r");
    if (f == NULL) {
        printf("Failed to open file to reading\n");
        return;
    }
    char line[64];
    while (fgets(line, sizeof(line), f)) {
        printf("Read: %s", line);

    }
    fclose(f);

    //delete file   
    if (remove("/spiffs/hello.txt")==0) {
        printf("File deleted succesfully\n");
    } else {
        printf("Failed to delete file\n");
    }
    //unmount SPIFFS    
    esp_vfs_spiffs_unregister(NULL);
    printf("SPIFFS unmounted\n");
}
