#include "hardware/uart.h"
#include "hardware/irq.h"
#include "hardware/regs/intctrl.h"
#include "pico/stdlib.h"

#include "debug.h"
#include <stdint.h>
#include <string.h>

#define UART_ID uart0
#define UART_IRQ_ID UART0_IRQ
#define UART_BAUD_RATE 115200

#define UART_TX_PIN 0
#define UART_RX_PIN 1

static int send_c = 0;

void send_keycodes(uint8_t hid_keycode[6], uint8_t hid_mod_code) {

  static uint8_t last_sent_keycodes[6] = {0};
  static uint8_t last_sent_mod = 0;

  if (memcmp(last_sent_keycodes, hid_keycode, 6) == 0 &&
      last_sent_mod == hid_mod_code) {
    return;
  }

  memcpy(last_sent_keycodes, hid_keycode, 6);
  last_sent_mod = hid_mod_code;

  uint8_t complete_keycodes[9] = {0};

  complete_keycodes[0] = 0xAA;
  complete_keycodes[1] = hid_keycode[0];
  complete_keycodes[2] = hid_keycode[1];
  complete_keycodes[3] = hid_keycode[2];
  complete_keycodes[4] = hid_keycode[3];
  complete_keycodes[5] = hid_keycode[4];
  complete_keycodes[6] = hid_keycode[5];
  complete_keycodes[7] = hid_mod_code;
  complete_keycodes[8] = 0xBB;

  uart_write_blocking(UART_ID, complete_keycodes, 9);
}

void receive_keycodes_polling(uint8_t *codes2p, uint8_t *mod2p) {
  static int c = 0;
  if (uart_is_readable(UART_ID)) {
    c++;

    uint8_t buffer[9] = {0};

    uart_read_blocking(UART_ID, buffer, 9);

    if (buffer[0] != 0xAA || buffer[8] != 0xBB)
      debug_log_int(0, 0, 0, "UART RECEIVED WRONG");

    codes2p[0] = buffer[1];
    memcpy(codes2p, &buffer[1], 6);
    *mod2p = buffer[7];
  }
}

void init_uart() {
  uart_init(UART_ID, UART_BAUD_RATE);
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
}
