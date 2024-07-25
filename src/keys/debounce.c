#include "debounce.h"
#include "hardware/gpio.h"
#include "pico/platform.h"
#include "pico/stdlib.h"
#include "pico/types.h"
#include <stdint.h>
#include <stdio.h>

#include "keys.h"
#include "oled.h"

void handle_debounce(uint32_t current_time, uint8_t active_keys[],
                     uint32_t debounce_keys[]) {
  for (int i = 0; i < 18; i++) {
    if (debounce_keys[i] > 0) {
      if (current_time > debounce_keys[i]) {
        // check if still released
        if (gpio_get(GPIO_NUMBERS[i])) {
          // still released
          render_font(100, 55, 2, 3, "Y", FONT_IBM_CGAthin);
          render_buffer();
          active_keys[i] = 0;
          debounce_keys[i] = 0;
        } else {
          debounce_keys[i] = 0;
        }
      }
    }
  }
}
