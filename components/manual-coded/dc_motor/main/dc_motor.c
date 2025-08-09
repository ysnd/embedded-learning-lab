#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"


#define IN1_PIN 2
#define IN2_PIN 16
#define IN3_PIN 17
#define IN4_PIN 5
#define ENR_PIN 4
#define ENL_PIN 18

void motor_backward() {
	gpio_set_level(IN1_PIN, 1);
    gpio_set_level(IN2_PIN, 0);
    gpio_set_level(IN3_PIN, 1);
    gpio_set_level(IN4_PIN, 0);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 900);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 900);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
}
void motor_stop() {
    gpio_set_level(IN1_PIN, 0);
    gpio_set_level(IN2_PIN, 0);
    gpio_set_level(IN3_PIN, 0);
    gpio_set_level(IN4_PIN, 0);
}

        
void motor_forward() {
	gpio_set_level(IN1_PIN, 0);
    gpio_set_level(IN2_PIN, 1);
    gpio_set_level(IN3_PIN, 0);
    gpio_set_level(IN4_PIN, 1);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 900);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 900);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
}

void motor_right() {
	gpio_set_level(IN1_PIN, 1);
    gpio_set_level(IN2_PIN, 0);
    gpio_set_level(IN3_PIN, 0);
    gpio_set_level(IN4_PIN, 1);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 900);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 900);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
}

void motor_left() {
	gpio_set_level(IN1_PIN, 0);
    gpio_set_level(IN2_PIN, 1);
    gpio_set_level(IN3_PIN, 1);
    gpio_set_level(IN4_PIN, 0);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 900);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 900);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
}


void app_main(void)
{
    gpio_config_t io_conf= {
        .pin_bit_mask = (1ULL << IN1_PIN) | 
						(1ULL << IN2_PIN) |
						(1ULL << IN3_PIN) |
						(1ULL << IN4_PIN),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&io_conf);
    
    //pwm
    ledc_timer_config_t t_conf = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .freq_hz = 1000,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&t_conf);

    ledc_channel_config_t ch_right = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = ENR_PIN,
        .duty = 0,
        .hpoint = 0 
    };
    ledc_channel_config(&ch_right);
    
    ledc_channel_config_t ch_left = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL_1,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = ENL_PIN,
        .duty = 0,
        .hpoint = 0 
    };
    ledc_channel_config(&ch_left);
    
    while (1) {
		motor_forward();
        vTaskDelay(pdMS_TO_TICKS(2000));
		motor_stop();
        vTaskDelay(pdMS_TO_TICKS(500));
		motor_backward();
        vTaskDelay(pdMS_TO_TICKS(2000));
		motor_stop();
        vTaskDelay(pdMS_TO_TICKS(500));
		motor_left();
        vTaskDelay(pdMS_TO_TICKS(2000));
		motor_stop();
        vTaskDelay(pdMS_TO_TICKS(500));
		motor_right();
        vTaskDelay(pdMS_TO_TICKS(2000));
        motor_stop();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
