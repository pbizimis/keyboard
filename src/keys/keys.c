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

#include "config.h"
#include "debounce.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hid.h"
#include "oled.h"
#include <stdint.h>

#ifdef MAIN_HALF
#define KEYMAP_HALF_START 0
#else
#define KEYMAP_HALF_START 5
#endif

#define KEYS_PER_HALF 18
uint8_t GPIO_NUMBERS[KEYS_PER_HALF] = {4,  5,  6,  7,  8,  9,  10, 11, 12,
                                       13, 14, 15, 16, 17, 22, 26, 27, 28};

uint8_t *active_keys;
uint32_t *debounce_keys;

enum ACTIVE_LAYER { FIRST, SECOND, THIRD, FOURTH, FIFTH, SIXTH };

struct active_modifier_keys_struct {
  bool shift;
};

void pressed_key(uint8_t key_number) {

  static uint8_t active_layer = FIRST;
  static struct active_modifier_keys_struct active_modifier_keys;
  active_modifier_keys.shift = false;

  uint8_t col = KEYMAP_HALF_START + (key_number % (COLS / 2));
  uint8_t row = KEYMAP_HALF_START + (key_number / (COLS / 2));

  uint16_t key_value;
  key_value = KEYMAP[active_layer][row][col];

  // Split modifier and key HID hex codes
  uint8_t hid_modifier_code = (key_value >> 8) & 0xFF;
  uint8_t hid_key_code = key_value & 0xFF;

  /*********/
  // Send to USB or KEYBOARD

  bool is_ascii_shift_layer = 0;

  // 0x02 is LSHIFT
  if (active_modifier_keys.shift || hid_modifier_code == 0x02) {
    is_ascii_shift_layer = true;
  }

  // minus 4 to elimante the offset that the HID codes start at 0x04 (A)
  char key_as_ascii = HID_TO_ASCII_MAP[hid_key_code - 4][is_ascii_shift_layer];

  char key_arr[] = {key_as_ascii, '\0'};

  render_font(0, 0, 2, 5, key_arr, FONT_IBM_BIOS);
  render_buffer();

  send_keyboard_report(hid_key_code, hid_modifier_code);
}

/****************/
void render_arr_2(int i, int val) {
  if (val)
    render_font(30 + i * 8, 20, 2, 3, "1", FONT_IBM_CGAthin);
  else
    render_font(30 + i * 8, 20, 2, 3, "0", FONT_IBM_CGAthin);
}
/****************/

void debounce_key(uint8_t key_number, uint32_t events) {

  /*
  for (int i = 0; i < 18; i++) {
    render_arr_2(i, active_keys[i]);
  }
  render_buffer();
    */

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

void gpio_callback(uint gpio, uint32_t events) {
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
    gpio_set_irq_enabled_with_callback(
        pin, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
  }
}

void init_keys(uint8_t *active_keys_, uint32_t *debounce_keys_) {
  active_keys = active_keys_;
  debounce_keys = debounce_keys_;
  init_gpio_keys();
}
