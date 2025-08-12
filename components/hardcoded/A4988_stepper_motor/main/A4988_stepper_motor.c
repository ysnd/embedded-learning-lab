#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"

#define STEP_PIN 16
#define DIR_PIN 4
#define EN_PIN 5
#define MS1_PIN 19
#define MS2_PIN 18
#define MS3_PIN 23

#define STEP_PER_REV 2048
#define PULSE_US 800

static void set_mode(uint8_t ms1, uint8_t ms2, uint8_t ms3) {
    gpio_set_level(MS1_PIN, ms1);
    gpio_set_level(MS2_PIN, ms2);
    gpio_set_level(MS3_PIN, ms3);
}

static void step_once(void) {
    gpio_set_level(STEP_PIN, 1);
    esp_rom_delay_us(PULSE_US);
    gpio_set_level(STEP_PIN, 0);
    esp_rom_delay_us(PULSE_US);
}

static void move_steps(int32_t steps) {
    if (steps>0) {
        gpio_set_level(DIR_PIN, 1);
    } else {
        gpio_set_level(DIR_PIN, 0);
    }
    if (steps <0) {
        steps = -steps;
    }
    for (int32_t i =0; i < steps; ++i) {
        step_once();
    }
}

void app_main(void)
{
    gpio_config_t io = {
        .pin_bit_mask = (1ULL << STEP_PIN) | (1ULL << DIR_PIN) | (1ULL << EN_PIN) | (1ULL << MS1_PIN) | (1ULL << MS2_PIN) | (1ULL << MS3_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io);

    gpio_set_level(EN_PIN, 0);

    while (1) {
        set_mode(0, 0, 0);
        move_steps(STEP_PER_REV);
        printf("2048 Step CW FULL-STEP\n");
        vTaskDelay(pdMS_TO_TICKS(2000)); 
        set_mode(0, 0, 0);
        move_steps(-STEP_PER_REV);          
        printf("2048 Step CCW FULL-STEP\n");
        vTaskDelay(pdMS_TO_TICKS(2000));
        set_mode(1, 0, 0);
        move_steps(STEP_PER_REV);
        printf("2048 Step CW HALF-STEP\n");
        vTaskDelay(pdMS_TO_TICKS(2000)); 
        set_mode(1, 0, 0);
        move_steps(-STEP_PER_REV);          
        printf("2048 Step CCW HALF-STEP\n");
        vTaskDelay(pdMS_TO_TICKS(2000));
        set_mode(0, 1, 0);
        move_steps(STEP_PER_REV);
        printf("2048 Step CW QUARTER-STEP\n");
        vTaskDelay(pdMS_TO_TICKS(2000)); 
        set_mode(0, 1, 0);
        move_steps(-STEP_PER_REV);          
        printf("2048 Step CCW QUARTER-STEP\n");
        vTaskDelay(pdMS_TO_TICKS(2000));
        set_mode(1, 1, 0);
        move_steps(STEP_PER_REV);
        printf("2048 Step CW EIGHTH-STEP\n");
        vTaskDelay(pdMS_TO_TICKS(2000)); 
        set_mode(1, 1, 0);
        move_steps(-STEP_PER_REV);          
        printf("2048 Step CCW EIGHTH-STEP\n");
        vTaskDelay(pdMS_TO_TICKS(2000));
        set_mode(1, 1, 1);
        move_steps(STEP_PER_REV);
        printf("2048 Step CW SIXTEENTH-STEP\n");
        vTaskDelay(pdMS_TO_TICKS(2000)); 
        set_mode(1, 1, 1);
        move_steps(-STEP_PER_REV);          
        printf("2048 Step CCW SIXTEENTH-STEP\n");
        vTaskDelay(pdMS_TO_TICKS(2000));
    }

}
