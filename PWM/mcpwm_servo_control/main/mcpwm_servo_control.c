#include <stdio.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/mcpwm_prelude.h"

#define SERVO 18
#define SERVO_FREQ 50
#define TIMER_RES 1000000UL
#define PERIOD_TICKS (TIMER_RES / SERVO_FREQ)

#define SERVO_MIN_US 500
#define SERVO_MAX_US 2500
#define SERVO_MID_US 1500

//mapping
static inline uint32_t angle_to_pulse_us(int angle) {
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;
    float frac = (float)angle/180.0f;
    return (uint32_t)(SERVO_MIN_US + frac * (SERVO_MAX_US - SERVO_MIN_US));
}



void app_main(void)
{
    printf("MCPWM Servo control");

    mcpwm_timer_handle_t timer = NULL;
    mcpwm_timer_config_t timer_cfg = {
        .group_id = 0,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = TIMER_RES,
        .period_ticks = PERIOD_TICKS
    };
    ESP_ERROR_CHECK(mcpwm_new_timer(&timer_cfg, &timer));

    mcpwm_oper_handle_t oper = NULL;
    mcpwm_operator_config_t op_cfg = { .group_id = 0 };
    ESP_ERROR_CHECK(mcpwm_new_operator(&op_cfg, &oper));
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(oper, timer));

    mcpwm_cmpr_handle_t comparator = NULL;
    mcpwm_comparator_config_t cmp_cfg = {.flags.update_cmp_on_tez = true };
    ESP_ERROR_CHECK(mcpwm_new_comparator(oper, &cmp_cfg, &comparator));

    mcpwm_gen_handle_t gen = NULL;
    mcpwm_generator_config_t gen_cfg = { .gen_gpio_num = SERVO };
    ESP_ERROR_CHECK(mcpwm_new_generator(oper, &gen_cfg, &gen));

    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(gen, MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(gen, MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparator, MCPWM_GEN_ACTION_LOW)
    ));

    ESP_ERROR_CHECK(mcpwm_timer_enable(timer));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));

    uint32_t mid = SERVO_MID_US;
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, mid));
    printf("Initial pulse: %lu us (center)\n", mid);

    while (1) {
        printf("Sweep 0 -> 180\n");
        for (int a = 0; a <= 180; a+=5) {
            uint32_t pulse = angle_to_pulse_us(a);
            ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, pulse));
            vTaskDelay(pdMS_TO_TICKS(50));
        }
        vTaskDelay(pdMS_TO_TICKS(500));
        printf("Sweep 180 -> 0\n");
        for (int a = 180; a >= 0; a-=5) {
            uint32_t pulse = angle_to_pulse_us(a);
            ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, pulse));
            vTaskDelay(pdMS_TO_TICKS(50));
        }
        vTaskDelay(pdMS_TO_TICKS(500));
        printf("Set Center\n");
        ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, SERVO_MID_US));
        vTaskDelay(pdMS_TO_TICKS(1500));
    }
}
