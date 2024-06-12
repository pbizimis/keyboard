#include "hardware/uart.h"
#include "hardware/irq.h"
#include "pico/stdlib.h"

#include "oled.h"

#define UART_ID uart0
#define UART_IRQ_ID UART0_IRQ
#define UART_BAUD_RATE 115200

#define UART_TX_PIN 0
#define UART_RX_PIN 1

void uart_rx_irq() {
  // unsure why while and not if because of interrupt
  while (uart_is_readable(UART_ID)) {
    uint8_t ch = uart_getc(UART_ID);
    char ch_a[] = {53, ch};
    render_font(80, 50, 2, 5, ch_a, FONT_IBM_CGAthin);
    render_buffer();
  }
}

void setup_receiver() {
  irq_set_exclusive_handler(UART_IRQ_ID, uart_rx_irq);
  irq_set_enabled(UART_IRQ_ID, true);

  uart_set_irq_enables(UART_ID, true, false);
}

void setup_sender() {}

void init_uart() {
  uart_init(UART_ID, UART_BAUD_RATE);
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

  uart_set_fifo_enabled(UART_ID, false);

#if MAIN_HALF
  setup_receiver();
#else
  setup_sender();
#endif
}
