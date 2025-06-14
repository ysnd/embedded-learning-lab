#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define BUTTON1 2
#define LED1 4
#define BUTTON2 16
#define LED2 17

void task1(void *arg) {
        while (1) {
                int btn_state = gpio_get_level(BUTTON1);
                gpio_set_level(LED1, btn_state == 0 ? 1 : 0);
                vTaskDelay(pdMS_TO_TICKS(10));
		printf("Tombol 1 : %d\n", btn_state);
        }
}

void task2(void *arg) {
        while (1) {
                int btn_state = gpio_get_level(BUTTON2);
                gpio_set_level(LED2, btn_state == 0 ? 1 : 0);
                vTaskDelay(pdMS_TO_TICKS(10));
		printf("Tombol 2 : %d\n", btn_state);

        }
}


void app_main(void)
{

	gpio_reset_pin(LED1);
       	gpio_reset_pin(LED2);
	gpio_reset_pin(BUTTON1);
	gpio_reset_pin(BUTTON2);
	gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
	gpio_set_direction(LED2, GPIO_MODE_OUTPUT);
	gpio_set_direction(BUTTON1, GPIO_MODE_INPUT);
	gpio_set_direction(BUTTON2, GPIO_MODE_INPUT);
	gpio_set_pull_mode(BUTTON1, GPIO_PULLUP_ONLY);
	gpio_set_pull_mode(BUTTON2, GPIO_PULLUP_ONLY);

	xTaskCreate(task1, "Tombol 1", 2048, NULL, 1, NULL);
	xTaskCreate(task2, "Tombol 2", 2048, NULL, 1, NULL);	
}

