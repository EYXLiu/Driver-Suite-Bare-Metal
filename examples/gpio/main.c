#include <stdio.h>
#include <stdlib.h>
#include "gpio.h"
#include "pico/stdlib.h"

int main() {
  stdio_init_all();
  my_gpio_t *led = my_gpio_init(25, my_GPIO_MODE_OUTPUT);
  if (!led) {
      printf("Failed to initialize GPIO\n");
      return -1;
  } 
  while (true) {
    my_gpio_toggle(led);
    sleep_ms(500);
  }
  my_gpio_free(led);
  return 0;
}