cmake_minimum_required(VERSION 3.13)

project(driver_suite C CXX ASM)

add_executable(driver_suite
    src/main.c
    src/gpio.c
    src/timer.c
    src/uart.c
    src/spi.c
    src/i2c.c
    src/pwm.c
    src/adc.c
    src/dma.c
    src/system.c
)

target_include_directories(driver_suite PRIVATE include)