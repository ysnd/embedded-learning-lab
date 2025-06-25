#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "driver/dac.h"
#include <math.h>

#define DAC_CH DAC_CHANNEL_1
#define SAMPLE_COUNT 64 

uint8_t sine_table[SAMPLE_COUNT];

volatile int index = 0;

float note_freqs[] = {
    261.63, //do 
    293.66, //re
    329.63, //Mi 
    349.23,  //fa 
    392.00,  //so 
    440.00,        //la 
    493.88,        //si 
    523.25        //do
};

#define  NOTE_COUNT (sizeof(note_freqs)/sizeof(float))

void generate_sine_table() {
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        float theta = (2 * M_PI * i) / SAMPLE_COUNT;
        float sine = sinf(theta);
        sine_table[i] = (uint8_t)(128 + sine * 127);
    }
}

void timer_callback(void* arg) {
    dac_output_voltage(DAC_CH, sine_table[index]);
    index = (index + 1) % SAMPLE_COUNT;
}

esp_timer_handle_t tone_timer;

void play_note (float freq, int duration_ms) {
    index = 0;
    int interval_us = 1e6 / (freq * SAMPLE_COUNT);

    esp_timer_stop(tone_timer);
    esp_rom_delay_us(500);
    esp_timer_start_periodic(tone_timer, interval_us);

    vTaskDelay(pdMS_TO_TICKS(duration_ms));
    esp_timer_stop(tone_timer);
    

    
}


void app_main(void)
{
    dac_output_enable(DAC_CH);
    generate_sine_table();


    const esp_timer_create_args_t timer_args = {
        .callback  = &timer_callback,
        .name = "audio_dac_timer"
    };

    esp_timer_create(&timer_args, &tone_timer);


    while (1) {
        printf("do re mi\n");
        for (int i = 0; i < NOTE_COUNT ; i++) {
        play_note(note_freqs[i], 500);
        vTaskDelay(pdMS_TO_TICKS(100));
        printf("Play note[%d] = %.2f Hz\n", i, note_freqs[i]);
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
