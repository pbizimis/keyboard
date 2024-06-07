/*
 * Datasheet Page 10: 4-wire SPI
 * The serial interface consists of serial clock SCL (OLED_SCL_PIN), serial data
 * SI (OLED_SDA_PIN), A0 (OLED_DC_PIN) and CS (OLED_CS_PIN) . SI is shifted into
 * an 8-bit shift register on every rising edge of SCL in the order of D7, D6, …
 * and D0. A0 is sampled on every eighth clock and the data byte in the shift
 * register is written to the display data RAM or command register in the same
 * clock.
 * ...
 * The SPI is also able to work properly
 * when the CS always keep low, but it is not recommended
 *
 */

/*
 * Constant LOW and HIGH on CS and RST, are not optimal but work.
 * There are no free GPIO pins on the Pico.
 * Current test: CS constant, RST used on PIN 20
 */

// https://www.displayfuture.com/Display/datasheet/controller/SH1122.pdf

#include "fonts/IBM_BIOS.h"
#include "fonts/IBM_CGAthin.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico/time.h"
#include <stdint.h>
#include <stdio.h>

#define OLED_SCL_PIN 18
#define OLED_SDA_PIN 19
#define OLED_RST_PIN 20
#define OLED_DC_PIN 21

#define OLED_SET_DISPLAY_ON 0xAF
#define OLED_SET_CONTRAST 0x81
#define OLED_SET_ROW_ADDR 0xB0
#define OLED_SET_ENTIRE_DISPLAY_ON 0xA4
#define OLED_SET_ENTIRE_DISPLAY_OFF 0xA5
#define OLED_SET_REVERSE 0xA7

// #define OLED_BRIGHTNESS 0x
#define OLED_CONTRAST 0xFF // 0x00 to 0xFF

void setup_oled() {
  gpio_init(OLED_DC_PIN);
  gpio_set_dir(OLED_DC_PIN, GPIO_OUT);
  gpio_put(OLED_DC_PIN, 1);

  gpio_init(OLED_RST_PIN);
  gpio_set_dir(OLED_RST_PIN, GPIO_OUT);
  gpio_put(OLED_RST_PIN, 1);

  spi_init(spi0, 10 * 1000 * 1000);
  gpio_set_function(OLED_SCL_PIN, GPIO_FUNC_SPI);
  gpio_set_function(OLED_SDA_PIN, GPIO_FUNC_SPI);
}

void write_cmd(uint8_t cmd) {
  gpio_put(OLED_DC_PIN, 0);
  spi_write_blocking(spi0, &cmd, 1);
}

void write_data(uint8_t data) {
  gpio_put(OLED_DC_PIN, 1);
  spi_write_blocking(spi0, &data, 1);
}

uint8_t buffer[64][256];

void render_font(uint8_t x_offset, uint8_t y_offset, int8_t letter_spacing,
                 uint8_t space_spacing, char text[],
                 const char **FONT_BITMAP_ARRAY) {

  /*
   * Fonts are turned 90 degrees to the right
   * Number 5 as a bitmap
  00000000
  00100111
  01000101
  01000101
  01000101
  01000101
  00111001
  00000000
      */

  int empty_bits_in_row = 0;
  int letter_offset = 0;

  for (int i = 0; text[i] != '\0'; i++) {

    // Custom Space Width of 4 (3 + the space that comes after each letter)
    if ((int)text[i] == 32) {
      letter_offset += space_spacing - 1;
      continue;
    }

    const char *letter_bitmap = FONT_BITMAP_ARRAY[(int)text[i] - 32];

    for (int j = 0; j < 8; j++) {
      for (int k = 0; k < 8; k++) {

        if (letter_bitmap[j] & 1 << k) {
          buffer[y_offset + k][x_offset + letter_offset] = 1;
        } else {
          buffer[y_offset + k][x_offset + letter_offset] = 0;
          empty_bits_in_row++;
        }
      }
      if (!(empty_bits_in_row == 8)) {
        letter_offset++;
      }
      empty_bits_in_row = 0;
    }

    letter_offset += letter_spacing;
  }
}

void clear_buffer(uint8_t buffer[64][256]) {
  for (int i = 0; i < 64; i++) {
    for (int j = 0; j < 256; j++) {
      buffer[i][j] = 0;
    }
  }
}

void render_buffer(const uint8_t buffer[64][256]) {

  bool active = 0;
  uint8_t first_byte;

  for (int i = 0; i < 64; i++) {
    for (int j = 0; j < 256; j++) {

      if (!(j % 2)) {
        if (first_byte && buffer[i][j])
          write_data(0xFF);
        else if (!first_byte && buffer[i][j])
          write_data(0x0F);
        else if (first_byte && !buffer[i][j])
          write_data(0xF0);
        else if (!first_byte && !buffer[i][j])
          write_data(0x00);
      } else {
        first_byte = buffer[i][j];
      }
    }
  }
}

int init_oled() {
  setup_oled();

  gpio_put(OLED_RST_PIN, 0);
  sleep_us(20);
  gpio_put(OLED_RST_PIN, 1);
  sleep_us(20);

  write_cmd(OLED_SET_ROW_ADDR);
  write_cmd(0x00);
  asm volatile("nop \n nop \n nop");
  write_cmd(OLED_SET_CONTRAST);
  write_cmd(OLED_CONTRAST);
  asm volatile("nop \n nop \n nop");
  write_cmd(OLED_SET_DISPLAY_ON);

  sleep_ms(1000);

  write_cmd(OLED_SET_CONTRAST);
  write_cmd(0xFF);

  int counter = 0;

  for (int i = 0; i < 64; i++) {
    for (int j = 0; j < 256 / 2; j++) {
      write_data(0x00);
    }
  }

  render_font(0, 0, 2, 3,
              "Hello World",
              IBM_CGAthin);
  render_font(0, 10, 1, 5, "Hello World", IBM_BIOS);

  render_buffer(buffer);
  // clear_buffer(buffer);

  sleep_ms(500);

  write_cmd(OLED_SET_CONTRAST);
  write_cmd(0x08);

  sleep_ms(500);

  write_cmd(OLED_SET_CONTRAST);
  write_cmd(0xFF);

  sleep_ms(500);

  /*
for (int i = 0; i < 64; i++) {
  for (int j = 0; j < 256; j++) {
    printf("%d", buffer[i][j]);
  }
  printf("\n");
}
*/
  return 0;
}
