#include <stdint.h>

#define KEYS_PER_HALF 18
static uint8_t GPIO_NUMBERS[KEYS_PER_HALF] = {
    4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 22, 26, 27, 28};

void init_keys();
void pressed_key(uint8_t primary_half[18],
                 uint8_t secondary_half_pressed_keys[6]);

void gpio_callback(uint8_t gpio, uint32_t events);

void poll_secondary_half(uint8_t keys[18]);
