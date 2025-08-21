# MLX90614 Non-Contact Temperature Sensor Component (ESP-IDF)

This component is a basic driver for MLX90614 non-contact temperature sensor using ESP-IDF.

## Features
- Detect temperature object 
- Detect temperature ambient
- Simple and reusable

## How to Use
1. Add the component to project.
2. Include and configure in the project :

```c
#Include "MLX90614.h"

mlx_conf_t mlx = {
    .i2c_port = I2C_NUM_0,
    .sda_pin = SDA_PIN,
    .scl_pin = SCL_PIN,
    .freq_hz = 100000
};

void app_main(void)
{
    mlx_init(&mlx);

    while (1) {
        float to = mlx_read_temperature_object();
        float ta = mlx_read_temperature_ambient();
        printf("Object : %.2f C | Ambient : %.2f C\n", to, ta);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


```

## Reference

| Function | Description |
| --- | --- |
| `mlx_init(mlx_conf_t *cfg)` | Init MLX90614 sensor & I2C |
| `mlx_read_temperature_object();`| Read temperature object |
| `mlx_read_temperature_ambient();` | Read temperature ambient |

## Notes
- See example usage in `../components/example/MLX_temperature_basic/`
- This component is a basic library for MLX90614 non-contact temperature sensor, created as part of the learning process on how to build modular components in ESP-IDF.







