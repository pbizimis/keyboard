#include <stdint.h>

void init_uart();
void send_keycodes(uint8_t hid_keycode[6], uint8_t hid_mod_code);
void receive_keycodes(uint8_t *codes2, uint8_t *mod2);
void receive_keycodes_polling(uint8_t *codes2p, uint8_t *mod2p);
