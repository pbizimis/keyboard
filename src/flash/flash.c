#include "hardware/flash.h"
#include "hardware/sync.h"
#include <stdint.h>
#include <stdio.h>

#include "flash.h"
#include "oled.h"

inline uint32_t *getAddressPersistent() {
  extern uint32_t ADDR_PERSISTENT[];
  return ADDR_PERSISTENT;
}

#define PIN_SIZE 0x02
#define WIFI_PASSWORD_SIZE 0x14

#define PIN_OFFSET FLASH_ADDR
#define WIFI_PASSWORD_OFFSET FLASH_ADDR + PIN_SIZE

uint32_t address;
uint32_t flash_offset;
const uint8_t *flash_contents;

void init_flash() {
  address = (uint32_t)ADDR_PERSISTENT;
  flash_offset = address - XIP_BASE;
  flash_contents = (const uint8_t *)address;
}

void erase_flash() {
  uint32_t interrupts = save_and_disable_interrupts();
  flash_range_erase(flash_offset, FLASH_SECTOR_SIZE);
  restore_interrupts(interrupts);
}

void write_flash(uint8_t flash_data[FLASH_PAGE_SIZE]) {

  uint32_t interrupts = save_and_disable_interrupts();
  flash_range_program(flash_offset, flash_data, FLASH_PAGE_SIZE);
  restore_interrupts(interrupts);
}

void read_flash(uint8_t flash_buffer[FLASH_PAGE_SIZE]) {

  uint32_t interrupts = save_and_disable_interrupts();
  for (size_t i = 0; i < FLASH_PAGE_SIZE; ++i) {
    flash_buffer[i] = flash_contents[i];
  }
  restore_interrupts(interrupts);
}

void save_to_flash() {

  uint8_t flash_buffer[FLASH_PAGE_SIZE] = {0};
  uint8_t new_flash_buffer[FLASH_PAGE_SIZE] = {0};

  init_flash();
  read_flash(flash_buffer);
  if (flash_buffer[0] == 0xFF)
    render_font(0, 0, 2, 5, "FF1", FONT_IBM_BIOS);
  erase_flash();
  read_flash(flash_buffer);
  if (flash_buffer[0] == 0xFF)
    render_font(0, 10, 2, 5, "FF2", FONT_IBM_BIOS);

  write_flash(new_flash_buffer);
  read_flash(flash_buffer);
  if (flash_buffer[0] == 0x00)
    render_font(0, 20, 2, 5, "NOW 0", FONT_IBM_BIOS);

  render_buffer();
}

void save_pin(char *pin) {}

void save_wifi_password(char *password) {}
