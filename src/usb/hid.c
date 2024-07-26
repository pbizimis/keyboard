#include "class/hid/hid.h"
#include "bsp/board.h"
#include "config.h"
#include "pico/platform.h"
#include "pico/time.h"
#include "usb_descriptors.h"
#include <stdint.h>

// For debugging and testing HID codes
struct tracker {
  uint layer;
  uint sec;
  uint thrd;
};

static struct tracker t = {0, 0, 0};

uint16_t get_next_hid() {
  if (t.thrd < 9)
    t.thrd++;
  else {
    t.thrd = 0;
    t.sec++;
  }

  if (t.sec > 3) {
    t.sec = 0;
    t.layer++;
  }

  if (t.layer > 2)
    return 0;

  return KEYMAP[t.layer][t.sec][t.thrd];
}

void send_value_down() {
  // Skip if HID is not ready yet
  if (!tud_hid_ready())
    return;

  tud_hid_mouse_report(REPORT_ID_MOUSE, 0x00, 0, 0, -100, 0);
}

void send_value_up() {
  // Skip if HID is not ready yet
  if (!tud_hid_ready())
    return;

  tud_hid_mouse_report(REPORT_ID_MOUSE, 0x00, 0, 0, 100, 0);
}

void send_value(uint32_t events) {
  if (events)
    send_value_up();
  else
    send_value_down();
}

// Debugging End

static void send_hid_report(uint8_t report_id) {
  // Skip if HID is not ready yet
  if (!tud_hid_ready())
    return;

  switch (report_id) {
  case REPORT_ID_KEYBOARD: {
    // Use to avoid sending multiple consecutive zero reports for keyboard
    static bool has_keyboard_key = false;

    if (!has_keyboard_key) {
      uint8_t keycode[6] = {0};
      uint16_t mod_key = get_next_hid();

      char mod = (mod_key >> 8) & 0xFF;
      char key = mod_key & 0xFF;

      keycode[0] = key;

      tud_hid_keyboard_report(REPORT_ID_KEYBOARD, mod, keycode);
      has_keyboard_key = true;
    } else if (has_keyboard_key) {
      tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
      has_keyboard_key = false;
    }
    break;
  }

  case REPORT_ID_MOUSE: {
    int8_t const delta = 5;

    tud_hid_mouse_report(REPORT_ID_MOUSE, 0x00, delta, delta, 100, 0);
    break;
  }

  default:
    break;
  }
}

static bool has_keyboard_key = false;

void send_keyboard_report(uint8_t *hid_keycode, uint8_t hid_mod_code) {
  if (!tud_hid_ready())
    return;

  tud_hid_keyboard_report(REPORT_ID_KEYBOARD, hid_mod_code, hid_keycode);
}

void send_mouse_report() {}

void hid_task(uint gpio) { send_hid_report(REPORT_ID_KEYBOARD); }

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report,
                                uint16_t len) {
  (void)instance;
  (void)len;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                               hid_report_type_t report_type, uint8_t *buffer,
                               uint16_t reqlen) {
  // Needs to stay
  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                           hid_report_type_t report_type, uint8_t const *buffer,
                           uint16_t bufsize) {
  if (report_type == HID_REPORT_TYPE_OUTPUT) {
    // Set keyboard LED e.g Capslock, Numlock etc...
    if (report_id == REPORT_ID_KEYBOARD) {
      // bufsize should be (at least) 1
      if (bufsize < 1)
        return;

      uint8_t const kbd_leds = buffer[0];

      if (kbd_leds & KEYBOARD_LED_CAPSLOCK) {
        // Capslock On: disable blink, turn led on
        board_led_write(true);
      } else {
        // Caplocks Off: back to normal blink
        board_led_write(false);
      }
    }
  }
}
