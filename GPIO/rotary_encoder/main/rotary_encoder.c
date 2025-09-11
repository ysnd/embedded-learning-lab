#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"


#define CLK_PIN 32
#define DT_PIN 33
#define SW_PIN 25

static int encoder_value = 0;

static void IRAM_ATTR encoder_isr(void* arg) {
    static uint32_t last_time = 0;
    uint32_t time = xTaskGetTickCountFromISR();
    if ((time - last_time) > pdMS_TO_TICKS(5)) {
        int clk_state = gpio_get_level(CLK_PIN);
        int dt_state = gpio_get_level(DT_PIN);

        if (clk_state == dt_state) {
            encoder_value++;
        } else {
            encoder_value--;
        }
        last_time = time;
    }
}


void app_main(void)
{
    gpio_reset_pin(CLK_PIN);
    gpio_reset_pin(DT_PIN);
    gpio_reset_pin(SW_PIN);
    gpio_set_direction(CLK_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(DT_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(SW_PIN, GPIO_MODE_INPUT);
    gpio_pullup_en(CLK_PIN);
    gpio_pullup_en(DT_PIN);
    gpio_pullup_en(SW_PIN);
    gpio_set_intr_type(CLK_PIN, GPIO_INTR_NEGEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(CLK_PIN, encoder_isr, NULL);

    while (1) {
        printf("Encoder Value: %d | SW: %d\n", encoder_value, gpio_get_level(SW_PIN));
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
