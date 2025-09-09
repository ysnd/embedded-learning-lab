# RC522 RFID Component (ESP-IDF)

This component is a basic driver for RC522 RFID using ESP-IDF.

## Features
- Read Card UID.
- Simple and reusable.

## How to Use
1. Add the component to project.
2. Include and configure in the project :

```c
#include "RC522_RFID.h"

 rc522_conf_t conf = {
        .miso_pin = 19,
        .mosi_pin = 23,
        .sck_pin  = 18,
        .cs_pin   = 5,
        .rst_pin  = 21,
        .clock_speed_hz = 5000000
    };
    rc522_init(&conf);

    uint8_t uid[4];
    while (1) {
        uint8_t current_uid[4];
        uint8_t result = detect_card(current_uid);

        if (result) {
            printf("Card detected! UID: %02X %02X %02X %02X\n", current_uid[0], current_uid[1], current_uid[2], current_uid[3]);
        } else {
            printf("No card detected.\n");
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
```


## Reference

| Function | Description |
| --- | --- |
| `rc522_init(rc522_conf_t *conf)` | Init RC522 RFID |
| `uint8_t detect_card(uint8_t *uid)`| Read UID Card |



## Notes
- See example usage in `../components/example/rc522_rfid_basic/`
- This component is a basic library for RC522 RFID, created as part of the learning process on how to build modular components in ESP-IDF.









