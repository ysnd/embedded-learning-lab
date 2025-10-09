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
    printf("SPIFFS mounted succesfully\n");
    //write files   
    const char *filename = "/spiffs/data.txt";
    FILE *f = fopen(filename, "w");
    if (f == NULL) {
        printf("Failed to open %s for writing\n", filename);
        return;
    }
    fprintf(f, "Line 1: Hello from ESP32 SPIFFS!\n");
    fprintf(f, "Line 2: This is a test!\n");
    fclose(f);
    printf("Data Written to %s\n", filename);

    //read file 
    f = fopen(filename, "r");
    if (f == NULL) {
        printf("Failed to open %s to reading\n", filename);
        return;
    }
    char line[64];
    while (fgets(line, sizeof(line), f)) {
        printf("Read: %s", line);

    }
    fclose(f);
    printf("=== End of file===\n");

    //add new data (append)
    f = fopen(filename, "a+");
    if (f == NULL) {
        printf("Failed to open %s for appending\n", filename);
        return;
    }
    fprintf(f, "Line 3: Append text at runtime\n");
    fprintf(f, "Line 4: SPIFFS append demo.\n");
    fclose(f);
    printf("Append new lines to %s\n", filename);

    //read again
    f = fopen(filename, "r");
    if (f==NULL) {
        printf("Failed to reoopen %s for reading\n", filename);
        return;
    }
    printf("\n===READING AFTER APPEND===\n");
    while (fgets(line, sizeof(line), f)) {
        printf("%s", line);
    }
    fclose(f);
    printf("==ENd of File==\n");

    //unmount SPIFFS    
    esp_vfs_spiffs_unregister(NULL);
    printf("SPIFFS unmounted\n");
}

