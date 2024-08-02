#include "debug.h"
#include "oled.h"
#include <stdint.h>
#include <string.h>
#include "stdio.h"

void debug_log_int(int x, int y, uint8_t num, char *prefix) {

  char buffer[20];
  sprintf(buffer, "%s: %d", prefix, num);

  render_font(x, y, 2, 3, buffer, FONT_IBM_CGAthin);
  render_buffer();
}

void debug_log_array(int x, int y, uint8_t *combined_codes, int array_length, char *prefix) {
  char log[200];
  char buffer[20];
  int i;

  strcpy(log, prefix);

  for (i = 0; i < array_length; i++) {
    sprintf(buffer, "%d, ", combined_codes[i]);
    strcat(log, buffer);
  }

  render_font(x, y, 2, 5, log, FONT_IBM_CGAthin);
  render_buffer();
}
