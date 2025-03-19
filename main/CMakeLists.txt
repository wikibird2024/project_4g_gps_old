cmake_minimum_required(VERSION 3.16)
idf_component_register(SRCS "testmpu6050.c"
                    INCLUDE_DIRS "."
                    REQUIRES espressif__mpu6050)
