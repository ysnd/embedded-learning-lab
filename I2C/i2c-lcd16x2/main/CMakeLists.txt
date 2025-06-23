idf_component_register(SRCS "i2c-lcd16x2.c"
                    INCLUDE_DIRS "."
                    REQUIRES i2cdev hd44780)
