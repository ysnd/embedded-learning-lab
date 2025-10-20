#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

int seg_pins[] = {33, 25, 32, 2, 4, 16, 17, 5};

uint8_t numbers[11][8] = {
    {1,1,1,1,1,1,0,0},//0
    {0,1,1,0,0,0,0,0},//1 
    {1,1,0,1,1,0,1,0},//2 
    {1,1,1,1,0,0,1,0},//3 
    {0,1,1,0,0,1,1,0},//4 
    {1,0,1,1,0,1,1,0},//5
    {1,0,1,1,1,1,1,0},//6 
    {1,1,1,0,0,0,0,0},//7
    {1,1,1,1,1,1,1,0},//8 
    {1,1,1,1,0,1,1,0},//9
    {0,0,0,0,0,0,0,1}//dot

};

void display_digit(uint8_t digit) {
    for (int i = 0; i < 8; i++) {
        gpio_set_level(seg_pins[i], numbers[digit][i]);
    }
}


void app_main(void)
{
    for (int i = 0; i < 8; i++) {
        gpio_reset_pin(seg_pins[i]);
        gpio_set_direction(seg_pins[i], GPIO_MODE_OUTPUT);
    }

    while (1) {
        for (int i = 0; i<= 10; i++) {
            printf("displaying %d\n", i);
            display_digit(i);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}
