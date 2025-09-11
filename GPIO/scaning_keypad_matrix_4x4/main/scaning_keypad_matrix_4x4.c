#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define ROWS 4
#define COLS 4

int rowPins[ROWS] = {21, 19, 18, 5};
int colPins[COLS] = {15, 4, 2, 0};

char keys[ROWS][COLS] = {
    {'*', '7', '4', '1'},
    {'0', '8', '5', '2'},
    {'#', '9', '6', '3'},
    {'D', 'C', 'B', 'A'}
};

void keypad_init() {
    for (int i = 0; i < ROWS; i++) {
        gpio_reset_pin(rowPins[i]);
        gpio_set_direction(rowPins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(rowPins[i], 1);
    }
    for (int j = 0; j < COLS; j ++) {
        gpio_reset_pin(colPins[j]);
        gpio_set_direction(colPins[j], GPIO_MODE_INPUT);
        gpio_set_level(colPins[j], 1);
    }
}

void scan_keypad_task(void *pvParameters) {
    while (1) {
        for (int r = 0; r < ROWS; r++) {
            gpio_set_level(rowPins[r], 0);
            for (int c = 0; c < COLS; c++) {
                if (gpio_get_level(colPins[c])==0) {
                    printf("Tombl %c ditekan\n", keys[r][c]);
                    vTaskDelay(pdMS_TO_TICKS(200));
                }
            }
            gpio_set_level(rowPins[r], 1);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_main(void)
{
    keypad_init();
    xTaskCreate(scan_keypad_task, "scan keypad", 2084, NULL, 5, NULL);
}
