#include <stdint.h>

// The address to the persistent 4k flash storage
extern uint32_t ADDR_PERSISTENT[];

void save_to_flash();
