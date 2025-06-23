idf_component_register(SRCS "peripheral-i2c-lcd16x2.c"
                    INCLUDE_DIRS "."
                    REQUIRES i2cdev hd44780)
