#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED GPIO_NUM_4

void app_main(void)
{
	gpio_reset_pin(LED);
	gpio_set_direction(LED, GPIO_MODE_OUTPUT);

	while(1) {
		gpio_set_level(LED, 1); //led on
		vTaskDelay(pdMS_TO_TICKS(200));

		gpio_set_level(LED, 0); //led off
		vTaskDelay(pdMS_TO_TICKS(200));
	}

}
