#include "dht11.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"
#include "driver/gpio.h"

static gpio_num_t dht_gpio;

void dht11_init(gpio_num_t gpio_num) {
    dht_gpio = gpio_num;
    gpio_reset_pin(dht_gpio);
}

bool dht11_read(dht11_data_t *data) {
    uint8_t raw[5] = {0};
    // kirim start sinyal
    gpio_set_direction(dht_gpio, GPIO_MODE_OUTPUT);
    gpio_set_level(dht_gpio, 0);// 
    esp_rom_delay_us(18000); //tahan low 40us
    gpio_set_level(dht_gpio, 1);
    esp_rom_delay_us(40); //tahan high 40us
    
    //menyiapkan baca data
    gpio_set_direction(dht_gpio, GPIO_MODE_INPUT);

    //tunggu respon sinyal (80us low + 80us high)
    while (gpio_get_level(dht_gpio) == 1) {} //tunggu low
    int64_t start = esp_timer_get_time();
    while (gpio_get_level(dht_gpio) == 0) {} //tunggu high
    while (gpio_get_level(dht_gpio) == 1) {} //tunggu low selesai

    //baca data 40 bit
    for (int i=0; i<40; i++) {
        // tunggu leading low 50us
        while(gpio_get_level(dht_gpio) == 0) {}

        //ukur durasi high
        start = esp_timer_get_time();
        while (gpio_get_level(dht_gpio) == 1) {}
        int duration = esp_timer_get_time() - start;

        //decode bit
        raw[i/8] <<=1;
        if (duration >50) raw[i/8] |=1;
        
        }
    if (raw[4] != (raw[0] + raw[1] + raw[2] + raw[3])) return false;

    data->humidity = raw[0] + raw[1]/10.0;
    data->temperature = raw[2] + raw[3]/10.0;
    return true;
}


