# DHT11 Component (ESP-IDF)

This component is a basic driver for DHT11 sensor using ESP-IDF.

## Features
- Read temperature
- Read humidity
- Simple and reusable

## How to Use
1. initialize the sensor :

```c
#include "dht11.h"

dht11_init(GPIO_NUM);
dht11_data_t data;

```

2. Read value sensor :

```c
while (1) {
    if (dht11_read(&data)) {
        printf("Temperature : %.1fC, Humidity : %.1f%%\n", data.temperature, data.humidity);
    } 
    else {
        printf("Baca data sensor gagal!\n");
    }
    vTaskDelay(pdMS_TO_TICKS(2000));
}
```

## Notes
- See example usage in `../components/example/dht11-basic/`
- This component is a basic library for dht11 sensor, created as part of the learning process on how to build modular components in ESP-IDF.





