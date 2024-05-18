#include "bsp/board.h"
#include "usb_descriptors.h"

static void send_hid_report(uint8_t report_id, uint32_t gpio) {
  // Skip if HID is not ready yet
  if (!tud_hid_ready())
    return;

  switch (report_id) {
  case REPORT_ID_KEYBOARD: {
    // Use to avoid sending multiple consecutive zero reports for keyboard
    static bool has_keyboard_key = false;

    if (gpio == 5) {
      uint8_t keycode[6] = {0};
      keycode[0] = HID_KEY_A;

      tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);
      has_keyboard_key = true;
    } else if (gpio == 10) {
      uint8_t keycode[6] = {0};
      keycode[0] = HID_KEY_B;

      tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);
      has_keyboard_key = true;
    } else {
      // Send empty key report if previously had key pressed
      if (has_keyboard_key) {
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
      }
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

void hid_task(uint gpio) {

  // Remote wakeup
  if (tud_suspended() && gpio == 5) {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    tud_remote_wakeup();
  } else {
    // Send the 1st of report chain, the rest will be sent by
    // tud_hid_report_complete_cb()
    send_hid_report(REPORT_ID_KEYBOARD, gpio);
  }
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report,
                                uint16_t len) {
  (void)instance;
  (void)len;

  uint8_t next_report_id = report[0] + 1;

  if (next_report_id < REPORT_ID_COUNT) {
    send_hid_report(next_report_id, board_button_read());
  }
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
