idf_component_register(
    SRCS "ultrasonic_sensor.c"
    INCLUDE_DIRS "include"
    REQUIRES driver
    PRIV_REQUIRES esp_timer
)
