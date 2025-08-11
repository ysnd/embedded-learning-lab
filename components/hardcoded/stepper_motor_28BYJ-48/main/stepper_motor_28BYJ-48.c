#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"


#define IN1 5
#define IN2 4
#define IN3 16
#define IN4 17


const uint8_t half_step_seq[8][4] = {
    {1,0,0,0},
    {1,1,0,0},
    {0,1,0,0},
    {0,1,1,0},
    {0,0,1,0},
    {0,0,1,1},
    {0,0,0,1},
    {1,0,0,1}
};
const uint8_t full_step_seq[4][4] = {
    {1,1,0,0},
    {0,1,1,0},
    {0,0,1,1},
    {1,0,0,1}
};

void move_stepper(int direction, int steps, int step_delay, int mode) {
    const uint8_t (*sequence)[4];
    int steps_per_cycle;
    if(mode ==1) {
        sequence = full_step_seq;
        steps_per_cycle = 4;
    } else {
        sequence = half_step_seq;
        steps_per_cycle = 8;
    }
   
    for (int i = 0; i < steps; i++) {
        int step_index = i % steps_per_cycle;

        int seq_index;
        if (direction ==1) {
            seq_index = step_index;
        } else {
            seq_index = (steps_per_cycle - 1) - step_index;
        }

        gpio_set_level(IN1, sequence[seq_index][0]);
        gpio_set_level(IN2, sequence[seq_index][1]);
        gpio_set_level(IN3, sequence[seq_index][2]);
        gpio_set_level(IN4, sequence[seq_index][3]);
        vTaskDelay(pdMS_TO_TICKS(step_delay));
    }
    gpio_set_level(IN1, 0);
    gpio_set_level(IN2, 0);
    gpio_set_level(IN3, 0);
    gpio_set_level(IN4, 0);
}

void stepper_init() {
    gpio_reset_pin(IN1);
    gpio_reset_pin(IN2);
    gpio_reset_pin(IN3);
    gpio_reset_pin(IN4);
    gpio_set_direction(IN1, GPIO_MODE_OUTPUT);
    gpio_set_direction(IN2, GPIO_MODE_OUTPUT);
    gpio_set_direction(IN3, GPIO_MODE_OUTPUT);
    gpio_set_direction(IN4, GPIO_MODE_OUTPUT);
    gpio_set_level(IN1, 0);
    gpio_set_level(IN2, 0);
    gpio_set_level(IN3, 0);
    gpio_set_level(IN4, 0);
}

void app_main(void)
{
    stepper_init();

    while (1) {
        printf("Half-step CW  lambat\n");
        move_stepper(1, 1024, 10, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("Half-step CCW cepat\n"); 
        move_stepper(0, 1024, 10, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("Full-step CW  lambat\n");
        move_stepper(1, 512, 10, 1); 
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("Full-step CCW cepat\n");
        move_stepper(0, 512, 10, 1); 
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        
    }
}
