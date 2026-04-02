#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define ENC_A_PIN   25
#define ENC_B_PIN   26

static const char *TAG = "ENCODER";

void app_main(void)
{
    // Init GPIO (NO INTERRUPT!)
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << ENC_A_PIN) | (1ULL << ENC_B_PIN),
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,  // ← NO INTERRUPT
    };
    gpio_config(&io_conf);
    
    ESP_LOGI(TAG, "Encoder polling ready");
    
    uint8_t last_state = 0;
    int32_t count = 0;
    
    while (1) {
        uint8_t a = gpio_get_level(ENC_A_PIN);
        uint8_t b = gpio_get_level(ENC_B_PIN);
        uint8_t curr = (a << 1) | b;
        
        if (curr != last_state) {
            static const int8_t table[16] = {
                0, -1, +1, 0,
                +1, 0, 0, -1,
                -1, 0, 0, +1,
                0, +1, -1, 0
            };
            
            uint8_t index = (last_state << 2) | curr;
            int8_t delta = table[index];
            if (delta != 0) {
                count += delta;
            }
            last_state = curr;
        }
        
        // Report per 4 counts (1 detent)
        if (count >= 4) {
            count -= 4;
            ESP_LOGI(TAG, "SCROLL: 1");
        } else if (count <= -4) {
            count += 4;
            ESP_LOGI(TAG, "SCROLL: -1");
        }
        
        vTaskDelay(1);  // 1kHz polling, yield ke FreeRTOS
    }
}
