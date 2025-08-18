# DS18B20 Temperature Sensor Component (ESP-IDF)

This component is a basic driver for reading DS18B20 temperature sensor using ESP-IDF.

## Features
- Read temperature in Celcius
- Range -55...+125 C
- Support one or multiple sensors
- Simple and reusable

## How to Use
1. initialize the sensor :

```c
#include "ds18b20.h"

ds18b20_init(sensor_pin);

```

2. Read temperature :

```c
float temperatur = ds18b20_read_temp(sensor_pin);
printf("Suhu : %.2f Celcius\n", temperatur);
vTaskDelay(pdMS_TO_TICKS(1000));
```

## Notes
- see example usage in `../components/example/ds18b20_basic`
- This component is a basic library for read temperature DS18B20 sensor, created as part of the learning process on how to build modular components in ESP-IDF.




