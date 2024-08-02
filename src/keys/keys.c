/*
 * +-----+                       +------+
 * |  1  | GP0   UART            | 40   | VBUS
 * |  2  | GP1   UART            | 39   | VSYS
 * |  3  | GND                   | 38   | GND
 * |  4  | GP2   ROTARY          | 37   | 3V3_EN
 * |  5  | GP3   ROTARY          | 36   | 3V3(OUT)
 * |  6  | GP4   KEY1            | 35   |
 * |  7  | GP5   KEY2            | 34   | GP28   KEY18
 * |  8  | GND                   | 33   | GND
 * |  9  | GP6   KEY3            | 32   | GP27   KEY17
 * | 10  | GP7   KEY4            | 31   | GP26   KEY16
 * | 11  | GP8   KEY5            | 30   | RUN
 * | 12  | GP9   KEY6            | 29   | GP22   KEY15
 * | 13  | GND                   | 28   | GND
 * | 14  | GP10  KEY7            | 27   | GP21   OLED (SPI)
 * | 15  | GP11  KEY8            | 26   | GP20   OLED (SPI)
 * | 16  | GP12  KEY9            | 25   | GP19   OLED (SPI)
 * | 17  | GP13  KEY10           | 24   | GP18   OLED (SPI)
 * | 18  | GND                   | 23   | GND
 * | 19  | GP14  KEY11           | 22   | GP17   KEY14
 * | 20  | GP15  KEY12           | 21   | GP16   KEY13
 * +-----+                       +------+
 */

/*
 * Left Half and Right Half (from left to right keys)
 * +-----+-----+-----+-----+-----+
 * |  0  |  1  |  2  |  3  |  4  |
 * | GP4 | GP5 | GP6 | GP7 | GP8 |
 * +-----+-----+-----+-----+-----+
 * |  5  |  6  |  7  |  8  |  9  |
 * | GP9 | GP10| GP11| GP12| GP13|
 * +-----+-----+-----+-----+-----+
 * | 10  | 11  | 12  | 13  | 14  |
 * | GP14| GP15| GP16| GP17| GP22|
 * +-----+-----+-----+-----+-----+
 *       | 15  | 16  | 17  |
 *       | GP26| GP27| GP28|
 *       +-----+-----+-----+
 */

#include "keys.h"
#include "config.h"
#include "debounce.h"
#include "debug.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hid.h"
#include "stdio.h"
#include "uart.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

uint8_t *active_keys;
uint32_t *debounce_keys;

enum ACTIVE_LAYER { FIRST, SECOND, THIRD, FOURTH, FIFTH, SIXTH };

#define KEY_PRESS_LIMIT 10

#ifndef MAIN_HALF
void poll_secondary_half(uint8_t keys[18]) {
  // I don't think someone would click more than 6 keys at the same time.
  // Even when gaming...
  uint8_t pressed_keys[6] = {0};
  uint8_t counter = 0;

  for (int i = 0; i < 18; i++) {
    if (counter == 7) {
      debug_log_int(0, 0, 0, "MORE THAN 6 KEYS ACTIVE");
      return;
    }
    if (keys[i]) {
      pressed_keys[counter++] = i + 1; // + 1 so that index 0 is 1
    }
  }

  send_keycodes(pressed_keys);
};
#endif

void get_layer_keys(uint8_t key, uint8_t *active_layer, uint8_t *pressed_keys,
                    uint8_t *counter, bool primary) {

  uint8_t offset = primary ? 0 : 5;

  uint8_t row = (key / (COLS / 2));
  uint8_t col = offset + (key % (COLS / 2));

  uint16_t key_value = KEYMAP[0][row][col];

  // Split modifier and key HID hex codes
  uint8_t hid_modifier_code = (key_value >> 8) & 0xFF;
  uint8_t hid_key_code = key_value & 0xFF;

  if (hid_modifier_code == 0x10) {
    // Layer key found
    *active_layer = hid_key_code;
  } else {
    // Normal key found
    if (*counter > 9) {
      debug_log_int(0, 0, 0,
                    "Samir, you are breaking the car (You are pressing more "
                    "than 10 keys)");
    } else {
      if (primary)
        pressed_keys[(*counter)++] =
            key + 1 + 5 * row; // + 1 to avoid 0 as a valid pressed key
      else
        pressed_keys[(*counter)++] =
            key + 1 + 5 * row + offset; // + 1 to avoid 0 as a valid pressed key
    }
  }
}

void pressed_key(uint8_t primary_half[18],
                 uint8_t secondary_half_pressed_keys[6]) {

  // First step: Filter each
  uint8_t active_layer = FIRST; // Default
  uint8_t pressed_keys[KEY_PRESS_LIMIT] = {0};
  uint8_t pressed_keys_counter = 0;

  // Extract all pressed keys and a layer key
  for (int i = 0; i < 18; i++) {
    if (primary_half[i]) {
      get_layer_keys(i, &active_layer, pressed_keys, &pressed_keys_counter, 1);
    }
  }

  for (int i = 0; i < 6; i++) {
    if (secondary_half_pressed_keys[i])
      get_layer_keys(secondary_half_pressed_keys[i] - 1, &active_layer,
                     pressed_keys, &pressed_keys_counter, 0);
  }

  uint8_t modifier = 0x00;
  uint8_t hid_codes[6] = {0};
  uint8_t hid_codes_counter = 0;
  uint8_t pressed_key_number;

  if (pressed_keys_counter == 0) {
    uint8_t empty[6] = {0};
    send_keyboard_report(empty, 0);
    return;
  }

  for (int i = 0; i < KEY_PRESS_LIMIT; i++) {

    if (pressed_keys[i] == 0)
      continue;

    pressed_key_number = pressed_keys[i] - 1;

    uint8_t col = pressed_key_number % COLS;
    uint8_t row = pressed_key_number / COLS;

    uint16_t key_value = KEYMAP[active_layer][row][col];

    // Split modifier and key HID hex codes
    uint8_t hid_modifier_code = (key_value >> 8) & 0xFF;
    uint8_t hid_key_code = key_value & 0xFF;

    if (hid_modifier_code == 0x20) {
      // Mod key found, hid_key_code has mod key code
      modifier = modifier | hid_key_code;
    } else {
      // Normal key found, optional mod code applied
      modifier = modifier | hid_modifier_code;

      hid_codes[hid_codes_counter++] = hid_key_code;
    }
  }

  send_keyboard_report(hid_codes, modifier);

  // FOR DISPLAY ON OLED

  /*
    // minus 4 to elimante the offset that the HID codes start at 0x04 (A)
    char key_as_ascii = HID_TO_ASCII_MAP[hid_key_code -
    4][is_ascii_shift_layer];

    char key_arr[] = {key_as_ascii, '\0'};

    */
}

void debounce_key(uint8_t key_number, uint32_t events) {

  if (events == 1 << 2) {
    if (active_keys[key_number])
      return;

    active_keys[key_number] = 1;

  } else if (events == 1 << 3) {
    if (active_keys[key_number] == 1 && debounce_keys[key_number] == 0) {
      debounce_keys[key_number] = time_us_32() + 5000; // delay by 5ms
    }
  }
};

void gpio_callback(uint8_t gpio, uint32_t events) {
  // switch case could be replaced by:
  // counter;
  // for n in GPIO_NUMBERS
  //      if (gpio == n)
  //          pressed_key(counter);
  //      counter++;

  switch (gpio) {
  case 4:
    debounce_key(0, events);
    break;
  case 5:
    debounce_key(1, events);
    break;
  case 6:
    debounce_key(2, events);
    break;
  case 7:
    debounce_key(3, events);
    break;
  case 8:
    debounce_key(4, events);
    break;
  case 9:
    debounce_key(5, events);
    break;
  case 10:
    debounce_key(6, events);
    break;
  case 11:
    debounce_key(7, events);
    break;
  case 12:
    debounce_key(8, events);
    break;
  case 13:
    debounce_key(9, events);
    break;
  case 14:
    debounce_key(10, events);
    break;
  case 15:
    debounce_key(11, events);
    break;
  case 16:
    debounce_key(12, events);
    break;
  case 17:
    debounce_key(13, events);
    break;
  case 22:
    debounce_key(14, events);
    break;
  case 26:
    debounce_key(15, events);
    break;
  case 27:
    debounce_key(16, events);
    break;
  case 28:
    debounce_key(17, events);
    break;
  default:
    // TODO
    // log error
  }
}

void init_gpio_keys() {
  for (int i = 0; i < KEYS_PER_HALF; i++) {
    uint pin = GPIO_NUMBERS[i];
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_up(pin); // Enable internal pull-up resistor
    // gpio_set_irq_enabled_with_callback(
    // pin, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
  }
}

void init_keys(uint8_t *active_keys_, uint32_t *debounce_keys_) {
  active_keys = active_keys_;
  debounce_keys = debounce_keys_;
  init_gpio_keys();
}
