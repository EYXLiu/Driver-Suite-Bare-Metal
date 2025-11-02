#include "gpio.h"
#include "timer.h"

#define LED_PIN 25

int main() {
  gpio_init(LED_PIN, GPIO_MODE_OUTPUT);

  while (true) {
    gpio_write(LED_PIN, GPIO_LOW);
    sleep_ms(500);
    gpio_write(LED_PIN, GPIO_HIGH);
    sleep_ms(500);
  }
}