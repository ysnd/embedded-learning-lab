#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define LED1 4
#define LED2 17
#define BUTTON1 2
volatile bool button_pressed = false;


static void IRAM_ATTR button_isr_handler(void* arg) {
	button_pressed = true;
}

void app_main(void)
{
	gpio_reset_pin(LED1);
	gpio_reset_pin(LED2);
	gpio_reset_pin(BUTTON1);
	gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
	gpio_set_direction(LED2, GPIO_MODE_OUTPUT);
	gpio_set_direction(BUTTON1, GPIO_MODE_INPUT);
	gpio_set_pull_mode(BUTTON1, GPIO_PULLUP_ONLY);

	gpio_set_intr_type(BUTTON1, GPIO_INTR_NEGEDGE);

	gpio_install_isr_service(0);
	gpio_isr_handler_add(BUTTON1, button_isr_handler, NULL);

	while(1) {
		gpio_set_level(LED2, 1);
		vTaskDelay(pdMS_TO_TICKS(500));
		gpio_set_level(LED2, 0);
		vTaskDelay(pdMS_TO_TICKS(500));
		if (button_pressed == 1) {
			gpio_set_level(LED1, 1);
			printf("Button Pressed\n");
		}
		else {
		      gpio_set_level(LED1, 0); 
		}
		button_pressed = false;
	}

}
