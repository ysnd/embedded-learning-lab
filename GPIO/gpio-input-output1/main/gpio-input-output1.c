#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED 4
#define BUTTON 2

void app_main(void)
{
	gpio_reset_pin(LED);

	gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
	gpio_set_direction(LED, GPIO_MODE_OUTPUT);
	gpio_set_pull_mode(BUTTON, GPIO_PULLUP_ONLY);

	while(1) {
		int button_state = gpio_get_level(BUTTON);
		printf("Button state: %d\n", button_state);

		if (button_state == 0) {
			gpio_set_level(LED, 1);
		}
		else {
			gpio_set_level(LED, 0);
		}
		vTaskDelay(pdMS_TO_TICKS(10));
	}

}
