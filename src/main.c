#include "pico/stdio.h"

#include "hardware/gpio.h"
#include "pico/cyw43_arch.h"

#include "hid.h"
#include "setup.h"
#include <stdbool.h>

#include "rotary.h"
#include "oled.h"

#define NUM_KEYS 2

const unsigned int keys[NUM_KEYS] = {5, 10};

void init_gpio_keys();

/*------------- MAIN -------------*/
int main(void) {
  stdio_init_all();

  setup_tiny_usb();

  init_gpio_keys();

  init_rotary();

  init_oled();

  if (cyw43_arch_init()) {
    return -1;
  }
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

  while (1) {
    tud_task_tiny_usb();
  }
}

//--------------------------------------------------------------------+
// Key init
//--------------------------------------------------------------------+

void gpio_callback(uint gpio, uint32_t events) {
  if (gpio == 5) {
    hid_task(5);
  } else if (gpio == 10) {
    hid_task(10);
  }
}

void init_gpio_keys() {
  for (int i = 0; i < NUM_KEYS; i++) {
    uint pin = keys[i];
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_up(pin); // Enable internal pull-up resistor
    gpio_set_irq_enabled_with_callback(pin, GPIO_IRQ_EDGE_FALL, true,
                                       &gpio_callback);
  }
}
