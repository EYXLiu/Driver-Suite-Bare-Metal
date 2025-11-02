cmake_minimum_required(VERSION 3.13)

project(driver_suite C CXX ASM)

add_executable(driver_suite
    src/gpio.c
    src/uart.c
    src/spi.c
    src/i2c.c
    src/timer.c
    src/adc.c
    src/pwm.c
    src/dma.c
    src/system.c
)

target_include_directories(driver_suite PRIVATE include)