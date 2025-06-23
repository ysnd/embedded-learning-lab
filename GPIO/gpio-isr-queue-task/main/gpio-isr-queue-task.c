#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#define BUTTON 2
#define LED 4

QueueHandle_t gpio_evt_queue = NULL;

//isr
static void IRAM_ATTR gpio_isr_handler(void* arg) {
	uint32_t gpio_num = (uint32_t) arg;
	xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}


void gpio_task(void* arg) {
	uint32_t io_num;
	while (1) {
		if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            printf("Interrupt dari GPIO %lu\n", io_num);
			gpio_set_level(LED, !gpio_get_level(LED));
		}
	}
}

void app_main(void) {
	gpio_reset_pin(BUTTON);
	gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
	gpio_set_pull_mode(BUTTON, GPIO_PULLUP_ONLY);
	gpio_set_intr_type(BUTTON, GPIO_INTR_NEGEDGE);
	
	gpio_reset_pin(LED);
	gpio_set_direction(LED, GPIO_MODE_OUTPUT);
	
	gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
	
	xTaskCreate(gpio_task, "gpio_task", 2048, NULL, 10, NULL);
	
	gpio_install_isr_service(0);
	gpio_isr_handler_add(BUTTON, gpio_isr_handler, (void*) BUTTON);
}
