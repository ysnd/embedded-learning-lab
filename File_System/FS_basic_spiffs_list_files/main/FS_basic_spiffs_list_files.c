#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_spiffs.h"

void list_spiffs_files(const char *base_path) {
    DIR *dir = opendir(base_path);
    if (!dir) {
        printf("Failed to open directory: %s\n", base_path);
        return;
    }
    printf("\n=== Listing files in %s ===\n", base_path);

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        char filepath[300];
        snprintf(filepath, sizeof(filepath), "%s/%s", base_path, entry->d_name);

        struct stat st;
        if (stat(filepath, &st)==0)
            printf("File: %-20s Size: %ld bytes\n", entry->d_name, st.st_size);
        else 
            printf("File: %-20s [Could not get info]\n", entry->d_name);
    }
    closedir(dir);
    printf("=== End of list ===\n\n");
}

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
    //create some files dummy
    for (int i= 1 ;i<=3; i++) {
        char filename[32];
        snprintf(filename, sizeof(filename), "/spiffs/test_%d.txt", i);
        FILE *f = fopen(filename, "w");
        if (f) {
            fprintf(f, "File number %d\n", i);
            fclose(f);
            printf("Created: %s\n",filename);
        }
    }

    // show list files
    list_spiffs_files("/spiffs");

    //unmount
    esp_vfs_spiffs_unregister(conf.partition_label);
    printf("SPIFFS unmounted\n");
}
