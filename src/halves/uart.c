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

void send_keycodes(uint8_t pressed_keys[6]) {
  static uint8_t last_sent_keycodes[6] = {0};

  if (memcmp(last_sent_keycodes, pressed_keys, 6) == 0) {
    return;
  }

  memcpy(last_sent_keycodes, pressed_keys, 6);

  uint8_t complete_keycodes[8] = {0};

  complete_keycodes[0] = 0xAA;
  complete_keycodes[1] = pressed_keys[0];
  complete_keycodes[2] = pressed_keys[1];
  complete_keycodes[3] = pressed_keys[2];
  complete_keycodes[4] = pressed_keys[3];
  complete_keycodes[5] = pressed_keys[4];
  complete_keycodes[6] = pressed_keys[5];
  complete_keycodes[7] = 0xBB;

  uart_write_blocking(UART_ID, complete_keycodes, 8);
}

void receive_keycodes_polling(uint8_t codes2p[6]) {
  if (uart_is_readable(UART_ID)) {
    uint8_t buffer[8] = {0};

    uart_read_blocking(UART_ID, buffer, 8);

    if (buffer[0] != 0xAA || buffer[7] != 0xBB)
      debug_log_int(0, 0, 0, "UART RECEIVED WRONG");

    memcpy(codes2p, &buffer[1], 6);
  }
}

void init_uart() {
  uart_init(UART_ID, UART_BAUD_RATE);
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
}
