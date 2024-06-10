#include <stdint.h>

enum FONT_TYPE { FONT_IBM_BIOS, FONT_IBM_CGAthin };

void init_oled();
void render_font(uint8_t x_offset, uint8_t y_offset, int8_t letter_spacing,
                 uint8_t space_spacing, char text[], enum FONT_TYPE font_type);

void render_buffer();
