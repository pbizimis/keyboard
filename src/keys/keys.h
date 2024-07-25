#include <stdint.h>

#define KEYS_PER_HALF 18
static uint8_t GPIO_NUMBERS[KEYS_PER_HALF] = {
    4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 22, 26, 27, 28};

void init_keys();
void pressed_key(uint8_t key_number);
