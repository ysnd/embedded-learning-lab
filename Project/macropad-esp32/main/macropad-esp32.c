#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "SSD1306.h"

#define ROWS 4
#define COLS 4
#define CLK_PIN 32
#define DT_PIN 33
#define SW_PIN 25

static volatile int encoder_value = 0;

int rowPins[ROWS] = {17, 19, 18, 5}; 
int colPins[COLS] = {15, 4, 2, 0};

char keys[ROWS][COLS] = {
    {'*', '7', '4', '1'},
    {'0', '8', '5', '2'},
    {'#', '9', '6', '3'},
    {'D', 'C', 'B', 'A'}
};

ssd1306_conf_t oled_conf = {
    .i2c_port = I2C_NUM_0,
    .sda_pin = 21,
    .scl_pin = 22,
    .freq_hz = 400000
};

static volatile int last_clk = 1;
static volatile int last_dt = 1;

static void IRAM_ATTR encoder_isr(void* arg) {
    int clk_state = gpio_get_level(CLK_PIN);
    int dt_state  = gpio_get_level(DT_PIN);

    if (clk_state != last_clk) {
        if (dt_state != clk_state) {
            if (encoder_value < 100) {
                encoder_value++;
            }
        } else {
            if (encoder_value > 0) {
            encoder_value--;
            }
        }
    }

    last_clk = clk_state;
    last_dt = dt_state;
}

void macropad_init() {
    for (int i = 0; i < ROWS; i++) {
        gpio_reset_pin(rowPins[i]);
        gpio_set_direction(rowPins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(rowPins[i], 1);
    }
    for (int j = 0; j < COLS; j ++) {
        gpio_reset_pin(colPins[j]);
        gpio_set_direction(colPins[j], GPIO_MODE_INPUT);
        gpio_set_pull_mode(colPins[j], GPIO_PULLUP_ONLY);
    }
    gpio_reset_pin(CLK_PIN);
    gpio_reset_pin(DT_PIN);
    gpio_reset_pin(SW_PIN);
    gpio_set_direction(CLK_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(DT_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(SW_PIN, GPIO_MODE_INPUT);
    gpio_pullup_en(CLK_PIN);
    gpio_pullup_en(DT_PIN);
    gpio_pullup_en(SW_PIN);
    gpio_set_intr_type(CLK_PIN, GPIO_INTR_ANYEDGE);
    gpio_set_intr_type(DT_PIN, GPIO_INTR_ANYEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(CLK_PIN, encoder_isr, NULL);
    gpio_isr_handler_add(DT_PIN, encoder_isr, NULL);
}

void scan_keypad_task(void *pvParameters) {
    while (1) {
        for (int r = 0; r < ROWS; r++) {
            gpio_set_level(rowPins[r], 0);

            for (int c = 0; c < COLS; c++) {
                if (gpio_get_level(colPins[c])==0) {
                    vTaskDelay(pdMS_TO_TICKS(20));
                    if (gpio_get_level(colPins[c])==0) {
                        printf("KEY:%c\n", keys[r][c]);
                        while (gpio_get_level(colPins[c]) ==0) {
                            vTaskDelay(pdMS_TO_TICKS(10));
                        }
                    }
                }
            }
            gpio_set_level(rowPins[r], 1);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void encoder_task(void *pvParameters) {
    int last_value = encoder_value;
    while (1) {
        if (encoder_value != last_value) {
            printf("ENC:%d\n", encoder_value);
            last_value = encoder_value;
        } 
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void rtsw_task(void *pvParameters) {
    while (1) {
        if (gpio_get_level(SW_PIN) == 0) {
            vTaskDelay(pdMS_TO_TICKS(20));
            if (gpio_get_level(SW_PIN) == 0) {
                printf("SW:PRESS\n");
                while (gpio_get_level(SW_PIN) == 0) {
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void oled_task(void *pvParameters) {
    ssd1306_init(&oled_conf);
    char line[64];

    while (1) {
        if (fgets(line, sizeof(line), stdin)) {
            ssd1306_clear();
            ssd1306_draw_string(0, 0, "     NET MONITOR     ", true);
            ssd1306_draw_string(0, 16, line, false);
            ssd1306_update_display();
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}


void app_main(void)
{
 macropad_init();
 xTaskCreate(scan_keypad_task, "scan keypad", 2048, NULL, 5, NULL);
 xTaskCreate(encoder_task, "encoder task", 2048, NULL, 5, NULL);
 xTaskCreate(rtsw_task, "rtsw task", 2048, NULL, 5, NULL);
 xTaskCreate(oled_task, "oled", 4096, NULL, 5, NULL);
} 
