/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bsp/board.h"
#include "hardware/gpio.h"
#include "pico/cyw43_arch.h"
#include "tusb.h"
#include "usb_descriptors.h"

#define NUM_KEYS 2

const unsigned int keys[NUM_KEYS] = {5, 10};

void hid_task(uint gpio);
void init_gpio_keys();

/*------------- MAIN -------------*/
int main(void) {
  stdio_init_all();
  board_init();
  tusb_init();
  init_gpio_keys();

  if (cyw43_arch_init()) {
    printf("Wi-Fi init failed");
    return -1;
  }
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

  while (1) {
    tud_task(); // tinyusb device task
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

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void) { printf("Device mounted"); }

// Invoked when device is unmounted
void tud_umount_cb(void) { printf("Device unmounted"); }

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) {
  (void)remote_wakeup_en;
  printf("Device suspended");
}

// Invoked when usb bus is resumed
void tud_resume_cb(void) { printf("Device resumed"); }

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

static void send_hid_report(uint8_t report_id, uint32_t gpio) {
  // skip if hid is not ready yet
  if (!tud_hid_ready())
    return;

  switch (report_id) {
  case REPORT_ID_KEYBOARD: {
    // use to avoid send multiple consecutive zero report for keyboard
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
      // send empty key report if previously has key pressed
      if (has_keyboard_key)
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
      has_keyboard_key = false;
    }
  } break;

  case REPORT_ID_MOUSE: {
    int8_t const delta = 5;

    // no button, right + down, no scroll, no pan
    tud_hid_mouse_report(REPORT_ID_MOUSE, 0x00, delta, delta, 0, 0);
  } break;
  default:
    break;
  }
}

// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc
// ..) tud_hid_report_complete_cb() is used to send the next report after
// previous one is complete
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
  // TODO not Implemented
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                           hid_report_type_t report_type, uint8_t const *buffer,
                           uint16_t bufsize) {
  (void)instance;

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
