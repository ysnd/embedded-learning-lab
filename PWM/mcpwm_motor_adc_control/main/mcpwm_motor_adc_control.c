#include <stdio.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/mcpwm_prelude.h"
#include "driver/gpio.h"
#include "driver/adc.h"

#define IN1 15
#define IN2 16
#define PWMA 17
#define POT ADC1_CHANNEL_5

void set_motor_direction(bool forward) {
    if (forward) {
        gpio_set_level(IN1, 1);
        gpio_set_level(IN2, 0);
    } else {
        gpio_set_level(IN1, 0);
        gpio_set_level(IN2, 1);
    }
}

void app_main(void) {
    printf("Init MCPWM  + ADC pot Control\n");

    //config direction pin  
    gpio_config_t io_cfg = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << IN1) | (1ULL << IN2),
    };
    gpio_config(&io_cfg);

    set_motor_direction(true);

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(POT, ADC_ATTEN_DB_12);

    //create timer
    mcpwm_timer_handle_t timer = NULL;
    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1 * 1000 * 1000,
        .period_ticks = 1000, //1khz PWM    
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };
    ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &timer));

    //create oprator
    mcpwm_oper_handle_t oper=NULL;
    mcpwm_operator_config_t operator_config= {
        .group_id = 0,
    };
    ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &oper));

    //connect operator to timer
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(oper, timer));
    //create comparator 
    mcpwm_cmpr_handle_t comparator = NULL;
    mcpwm_comparator_config_t compare_config = {
        .flags.update_cmp_on_tez = true,
    };
    ESP_ERROR_CHECK(mcpwm_new_comparator(oper,&compare_config, &comparator));

    //vreate generator PWM 
    mcpwm_gen_handle_t gen = NULL;
    mcpwm_generator_config_t gen_cfg = {
        .gen_gpio_num = PWMA,
    };

    ESP_ERROR_CHECK(mcpwm_new_generator(oper, &gen_cfg, &gen));
    //Event PWM 
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(gen, MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(gen, MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparator, MCPWM_GEN_ACTION_LOW)));

    ESP_ERROR_CHECK(mcpwm_timer_enable(timer));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));

    while (1) {
        int adc_raw = adc1_get_raw(POT);
        float duty = ((float)adc_raw / 4095.0f) * 100.0f;

        uint32_t compare_val = (uint32_t)(10.0f * duty);
        ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, compare_val));
        printf("ADC: %4d | duty: %3.1f%%\n", adc_raw, duty);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}



