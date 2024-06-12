#include "hardware/uart.h"
#include "pico/stdio.h"

#include "hardware/gpio.h"
#include "pico/cyw43_arch.h"

#include "hid.h"
#include "pico/time.h"
#include "setup.h"
#include <stdbool.h>

#include "flash.h"
#include "oled.h"
#include "rotary.h"
#include "uart.h"

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
  save_to_flash();

  init_uart();

  if (cyw43_arch_init()) {
    return -1;
  }
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

  int c2 = 30;

  while (1) {
    tud_task_tiny_usb();

#ifndef MAIN_HALF
    while (c2 < 120) {
      uart_putc(uart0, c2++);
      char sent[] = {83, c2};

      render_font(50, 40, 2, 5, sent, FONT_IBM_BIOS);
      render_buffer();
      sleep_ms(100);
    }

#endif
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
