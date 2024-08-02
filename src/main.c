#include "device/usbd.h"
#include "hardware/timer.h"
#include "hardware/uart.h"
#include "pico/stdio.h"

#include "hardware/gpio.h"
#include "pico/cyw43_arch.h"

#include "hid.h"
#include "pico/time.h"
#include "setup.h"
#include <stdbool.h>
#include <stdint.h>

#include "debounce.h"
#include "flash.h"
#include "keys.h"
#include "oled.h"
#include "rotary.h"
#include "uart.h"

#include "debug.h"

#define NUM_KEYS 2

const unsigned int keys[NUM_KEYS] = {5, 10};

static uint8_t active_keys[18] = {0};
static uint8_t active_keys_secondary[6] = {0};
static uint32_t debounce_keys[18] = {0};

/*------------- MAIN -------------*/
int main(void) {
  stdio_init_all();

  setup_tiny_usb();

  init_rotary();

  init_oled();
  save_to_flash();

  init_uart();

  init_keys(active_keys, debounce_keys);
  if (cyw43_arch_init()) {
    return -1;
  }
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

  uint32_t starting_time = time_us_32(); // in microseconds
  uint32_t next_time = starting_time + 1000000;
  int polling_rate = 0;

  int counter = 0;

  int log_arr[3] = {0};

  bool any_on;

  uint8_t hid1[6] = {0};
  uint8_t mod1 = 0;

  uint8_t codes2[6] = {0};
  while (1) {
    tud_task_tiny_usb();

    if (time_us_32() > next_time) {
      log_arr[counter] = polling_rate;

      polling_rate = 0;
      next_time += 1000000;

      if (counter++ == 1) {
        char str[200];
        sprintf(str, "LOG: %d, %d", log_arr[0], log_arr[1]);

        clear_buffer();
        render_font(60, 0, 2, 3, str, FONT_IBM_CGAthin);
        render_buffer();

        counter = 0;
      }
    }
    polling_rate++;

    /*
     *
     * For whatever reason there is a very small chance that the debounce is not
     * correctly assigned and a key keeps being activated even though it isn't.
     * I could only reproduce with very short switch activations about once
     * every hundred activations. Tested on two seperate switches on two
     * different GPIO pins. No idea why at the moment.
     *
     * Seems to be related to IRQ. Switched to polling and now I don't have the
     * problem. I will leave this comment for now in case it is still a problem.
     */

    for (int i = 0; i < 18; i++) {
      if (gpio_get(GPIO_NUMBERS[i])) {
        // not active
        gpio_callback(GPIO_NUMBERS[i], 1 << 3);
      } else {
        // active
        gpio_callback(GPIO_NUMBERS[i], 1 << 2);
      }
    }

    handle_debounce(time_us_32(), active_keys, debounce_keys);

#ifdef MAIN_HALF
    receive_keycodes_polling(active_keys_secondary);
    pressed_key(active_keys, active_keys_secondary);
#else
    poll_secondary_half(active_keys);
#endif
  }
}
