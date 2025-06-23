#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED1 4
#define BUTTON1 16
#define LED2 17
#define BUTTON2 2

void app_main(void)
{
        gpio_reset_pin(LED1);
	gpio_reset_pin(LED2);

        gpio_set_direction(BUTTON1, GPIO_MODE_INPUT);
	gpio_set_direction(BUTTON2, GPIO_MODE_INPUT);
        gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
	gpio_set_direction(LED2, GPIO_MODE_OUTPUT);
        gpio_set_pull_mode(BUTTON1, GPIO_PULLUP_ONLY);
	gpio_set_pull_mode(BUTTON2, GPIO_PULLUP_ONLY);

        while(1) {
                int button1_state = gpio_get_level(BUTTON1);
		int button2_state = gpio_get_level(BUTTON2);
                printf("Button 1 state: %d\n", button1_state);
		printf("Button 2 state: %d\n", button2_state);

                if (button1_state == 0) {
                        gpio_set_level(LED1, 1);
                }
		else {
                        gpio_set_level(LED1, 0);
                }
                if (button2_state == 0) {								gpio_set_level(LED2, 1);
                }

		else {
			gpio_set_level(LED2, 0);
		}
                vTaskDelay(pdMS_TO_TICKS(200));
        }

} 
