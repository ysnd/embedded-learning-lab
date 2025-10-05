#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/touch_pad.h"

void app_main(void)
{
    ESP_ERROR_CHECK(touch_pad_init());
    //config 2channel example t0(gpio4) & t3 (gpio15)
    ESP_ERROR_CHECK(touch_pad_config(TOUCH_PAD_NUM0, 0));
    ESP_ERROR_CHECK(touch_pad_config(TOUCH_PAD_NUM3, 0));

    //start filter builtin (optional, for noise stabilization)
    ESP_ERROR_CHECK(touch_pad_filter_start(10));

    while (1) {
        uint16_t val0, val3;

        //read raw value
        ESP_ERROR_CHECK(touch_pad_read_raw_data(TOUCH_PAD_NUM0, &val0));
        ESP_ERROR_CHECK(touch_pad_read_raw_data(TOUCH_PAD_NUM3, &val3));

        printf("Touchpad0(GPIO4)=%d, Touchpad3(GPIO15)=%d\n", val0, val3);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
