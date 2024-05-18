#include "bsp/board.h"
#include "device/usbd.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include <stdbool.h>

bool setup_tiny_usb() {
  board_init();
  tusb_init();
};

void tud_task_tiny_usb() {
    tud_task();
}
