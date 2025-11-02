#include "gpio.h"
#include "timer.h"

#define LED_PIN 25

int main() {
  gpio_t *gpio = gpio_init(LED_PIN, GPIO_MODE_OUTPUT);

  while (true) {
    gpio_write(gpio, GPIO_LOW);
    sleep_ms(500);
    gpio_write(gpio, GPIO_HIGH);
    sleep_ms(500);
  }
}