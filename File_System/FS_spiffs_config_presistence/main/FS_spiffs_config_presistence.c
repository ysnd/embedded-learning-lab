
#include <stdio.h>
#include <string.h>
#include "esp_err.h"
#include "esp_spiffs.h"

typedef struct {
    char device_name[32];
    float temp_threshold;
    int sampling_rate;
} config_t;

//fuction for write config to file
void save_cfg(const config_t *cfg) {
    FILE *f = fopen("/spiffs/config.txt", "w");
    if (f==NULL) {
        printf("Failed to open config files for writing\n");
        return;
    }
    fprintf(f, "%s\n%.2f\n%d\n", cfg->device_name, cfg->temp_threshold, cfg->sampling_rate);
    fclose(f);
    printf("Configuration stored succesfully\n");
}

//fuction for read configuration from file
bool load_cfg(config_t *cfg) {
    FILE *f = fopen("/spiffs/config.txt", "r");
    if (f == NULL) {
        printf("there is not Configuration file\n");
        return false;
    }
    fscanf(f, "%31s\n%f\n%d", cfg->device_name, &cfg->temp_threshold, &cfg->sampling_rate);
    fclose(f);
    printf("Configuration read succesfully from files\n");
    return true;
}

void app_main(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        printf("Failed mount SPIFFS (%s)\n", esp_err_to_name(ret));
        return;
    }
    printf("SPIFFS mounted.\n");

    config_t cfg;
    //loaod configuratioon files
    if (!load_cfg(&cfg)) {
        printf("Create configuration default...\n");
        strcpy(cfg.device_name, "ESP_32_logger");
        cfg.temp_threshold = 30.5;
        cfg.sampling_rate = 5;
        save_cfg(&cfg);
    }

    //display config
    printf("Device Name : %s\n",cfg.device_name);
    printf("Temp Threshold : %.2f\n", cfg.temp_threshold);
    printf("Sampling rate : %d s\n", cfg.sampling_rate);

    cfg.temp_threshold += 1.0;
    save_cfg(&cfg);
    esp_vfs_spiffs_unregister(NULL);
}

