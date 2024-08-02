#include <stdint.h>

void init_uart();
void send_keycodes(uint8_t keys[6]);
void receive_keycodes_polling(uint8_t codes2p[6]);
