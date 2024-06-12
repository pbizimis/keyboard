#include "hardware/flash.h"
#include "hardware/sync.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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

enum CONTRAST_LEVEL { LOW, MID, HIGH };
enum SYSTEM_STATUS { USER, FACTORY = 0xFF };

struct KEYBOARD_SETTINGS {
  enum CONTRAST_LEVEL contrast_level;
};

struct KEYBOARD_CONFIG {
  enum SYSTEM_STATUS system_status;
  uint8_t pin[4];
  char wifi_password[40];
  struct KEYBOARD_SETTINGS settings;
};

void save_config_to_flash(struct KEYBOARD_CONFIG *keyboard_config) {
  uint8_t config_buffer[FLASH_PAGE_SIZE];

  config_buffer[0] = (uint8_t)keyboard_config->system_status;

  config_buffer[1] = keyboard_config->pin[0];
  config_buffer[2] = keyboard_config->pin[1];
  config_buffer[3] = keyboard_config->pin[2];
  config_buffer[4] = keyboard_config->pin[3];

  memcpy(config_buffer + 5, keyboard_config->wifi_password, 40);

  config_buffer[45] = (uint8_t)keyboard_config->settings.contrast_level;

  write_flash(config_buffer);
}

void read_config_from_flash(struct KEYBOARD_CONFIG *keyboard_config) {
  uint8_t config_buffer[FLASH_PAGE_SIZE];
  read_flash(config_buffer);

  keyboard_config->system_status = (enum SYSTEM_STATUS)config_buffer[0];

  keyboard_config->pin[0] = config_buffer[1];
  keyboard_config->pin[1] = config_buffer[2];
  keyboard_config->pin[2] = config_buffer[3];
  keyboard_config->pin[3] = config_buffer[4];

  memcpy(keyboard_config->wifi_password, config_buffer + 5, 40);

  keyboard_config->settings.contrast_level =
      (enum CONTRAST_LEVEL)config_buffer[45];
}

struct KEYBOARD_CONFIG *init_config() {
  struct KEYBOARD_CONFIG *keyboard_config;
  keyboard_config = malloc(sizeof(*keyboard_config));

  read_config_from_flash(keyboard_config);

  if (keyboard_config->system_status != USER) {
    // TODO
    // keyboard_setup(keyboard_config);
    // call a function that goes through some of the settings such as:
    // - setting a pin
    // - setting a wifi password
    // - setting an oled contrast
    keyboard_config->system_status = USER;
    keyboard_config->pin[0] = 1;
    keyboard_config->pin[1] = 2;
    keyboard_config->pin[2] = 3;
    keyboard_config->pin[3] = 4;

    save_config_to_flash(keyboard_config);
  }

  return keyboard_config;
}

void save_pin(char *pin) {}

void save_wifi_password(char *password) {}

void save_to_flash() {
  // Temporary function for testing

  uint8_t flash_buffer[FLASH_PAGE_SIZE] = {0};

  init_flash();

  read_flash(flash_buffer);
  // This should only be true after nuking flash
  // or a new device
  if (flash_buffer[0] == 0xFF)
    render_font(0, 0, 2, 5, "FF", FONT_IBM_BIOS);
  render_buffer();

  struct KEYBOARD_CONFIG *keyboard_config = init_config();

  read_flash(flash_buffer);
  if (flash_buffer[0] == 0x00)
    render_font(0, 10, 2, 5, "User setup", FONT_IBM_BIOS);

#ifdef MAIN_HALF
  if (flash_buffer[2] == 0x02)
    render_font(0, 20, 2, 5, "MAIN_HALF", FONT_IBM_BIOS);
#else
  if (flash_buffer[2] == 0x02)
    render_font(0, 20, 2, 5, "NOT_MAIN_HALF", FONT_IBM_BIOS);
#endif

  render_buffer();
}
