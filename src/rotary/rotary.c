#include "hardware/gpio.h"
#include <stdint.h>

// TEMP
#include "hid.h"

#define CLK 3
#define DT 4

static int last_dir;

void log_rotary_signals(uint gpio, uint32_t events) {
  if (gpio_get(CLK) == gpio_get(DT)) {
    if (last_dir == 1)
      send_value(1);
    last_dir = 1;
  } else {
    if (last_dir == 0)
      send_value(0);
    last_dir = 0;
  }
}

void init_gpio(uint gpio) {
  gpio_init(gpio);
  gpio_pull_up(gpio);
}

void init_rotary() {
  init_gpio(CLK);
  init_gpio(DT);

  gpio_set_irq_enabled_with_callback(
      CLK, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &log_rotary_signals);
}
