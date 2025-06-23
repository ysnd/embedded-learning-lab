#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_timer.h"

#define LED1 4
#define BTN_INC 2
#define BTN_DEC 16

#define DEBOUNCE_TIME_US 200000
#define DUTY_STEP 20    
#define DUTY_MAX 255
#define DUTY_MIN 0
static int duty = 0;

QueueHandle_t gpio_evt_queue = NULL;

static uint64_t last_press_time[GPIO_NUM_MAX] = {0};

static void IRAM_ATTR gpio_isr_handler(void* arg) {
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

void gpio_task(void* arg) {
    uint32_t io_num;
    while (1) {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            uint64_t now = esp_timer_get_time();
            if (now - last_press_time[io_num] < DEBOUNCE_TIME_US) continue;
            last_press_time[io_num] = now;

            if (io_num == BTN_INC && duty < DUTY_MAX) {
                duty += DUTY_STEP;
                if (duty > DUTY_MAX) duty = DUTY_MAX;
            
            }
            else if (io_num == BTN_DEC && duty > DUTY_MIN) {
                duty -= DUTY_STEP;
                if (duty < DUTY_MIN) duty = DUTY_MIN;
            }
            
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, (uint32_t)duty);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
            printf("PWM Duty Now: %d\n", duty);
        }
    
    }
}
   
void setup_gpio_input(gpio_num_t gpio) {
    gpio_reset_pin(gpio);
    gpio_set_direction(gpio, GPIO_MODE_INPUT);
    gpio_set_pull_mode(gpio, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(gpio, GPIO_INTR_NEGEDGE);
}

void app_main(void)
{
     ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num  = LEDC_TIMER_0,
        .duty_resolution    = LEDC_TIMER_8_BIT,
        .freq_hz    = 1000,
        .clk_cfg    = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);
    
    ledc_channel_config_t ledc_channel = {
        .gpio_num   = LED1,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel    = LEDC_CHANNEL_0,
        .timer_sel  = LEDC_TIMER_0,
        .duty   = 0,
        .hpoint = 0
    };
    ledc_channel_config(&ledc_channel);

    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    
    setup_gpio_input(BTN_INC);
    setup_gpio_input(BTN_DEC);
    
    xTaskCreate(gpio_task, "gpio_task", 2048, NULL, 10, NULL);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BTN_INC, gpio_isr_handler, (void*)BTN_INC);
    gpio_isr_handler_add(BTN_DEC, gpio_isr_handler, (void*) BTN_DEC);
}
