/*
 * +----+                            +----+
 * |  1 | GP0                        | 40 | VBUS
 * |  2 | GP1                        | 39 | VSYS
 * |  3 | GND                        | 38 | GND
 * |  4 | GP2                        | 37 | 3V3_EN
 * |  5 | GP3                        | 36 | 3V3(OUT)
 * |  6 | GP4                        | 35 |
 * |  7 | GP5                        | 34 | GP28
 * |  8 | GND                        | 33 | GND
 * |  9 | GP6                        | 32 | GP27
 * | 10 | GP7                        | 31 | GP26
 * | 11 | GP8                        | 30 | RUN
 * | 12 | GP9                        | 29 | GP22
 * | 13 | GND                        | 28 | GND
 * | 14 | GP10                       | 27 | GP21
 * | 15 | GP11                       | 26 | GP20
 * | 16 | GP12                       | 25 | GP19
 * | 17 | GP13                       | 24 | GP18
 * | 18 | GND                        | 23 | GND
 * | 19 | GP14                       | 22 | GP17
 * | 20 | GP15                       | 21 | GP16
 * +----+                            +----+
 */

#include <stdint.h>

#define K_ADD_MOD(MOD, KEY)                         (MOD << 8 | KEY)
/*
 * Example:
 * #define K_1_EXCL 0x1E
 * #define K_MOD_LSHIFT 0x02
 * K_ADD_MOD(K_MOD_LSHIFT, K_1_EXCL) (MOD << 8 | KEY)
 * Calculation:
 * 0x02 << 8 == 00000010 << 8 == 0000001000000000 == 0x0200
 * 0x1E == 00011110
 * 0000001000000000 | 00011110 == 0000001000011110 == 0x021E
 * Both hex values are combined: 0x021E
 */

/* Modifiers */

#define K_MOD_INDICATOR                             0x20

#define K_MOD_EMPTY                                 0x00
#define K_MOD_LCTRL                                 0x01
#define K_MOD_LSHIFT                                0x02
#define K_MOD_LALT                                  0x04
#define K_MOD_LGUI                                  0x08

// To distinguish between mod keys and normal keys,
// I add the indicator since when a mod key is pressed
// alone, I don't know if it e.g. is the 0x08 of the Modifiers
// or the letter E

#define K_MOD_LCTRL_KEYMAP                          K_ADD_MOD(K_MOD_INDICATOR, K_MOD_LCTRL)
#define K_MOD_LSHIFT_KEYMAP                         K_ADD_MOD(K_MOD_INDICATOR, K_MOD_LSHIFT)
#define K_MOD_LALT_KEYMAP                           K_ADD_MOD(K_MOD_INDICATOR, K_MOD_LALT)
#define K_MOD_LGUI_KEYMAP                           K_ADD_MOD(K_MOD_INDICATOR, K_MOD_LGUI)

/* Layer */

#define K_MOD_TO_LAYER                              0x10 // Reserve 0x10 as the mod layer key; this value is not based on anything
#define K_TO_LAYER_1                                K_ADD_MOD(K_MOD_TO_LAYER, 0x01)
#define K_TO_LAYER_2                                K_ADD_MOD(K_MOD_TO_LAYER, 0x02)

/* Letters */

#define K_A                                         0x04
#define K_B                                         0x05
#define K_C                                         0x06
#define K_D                                         0x07
#define K_E                                         0x08
#define K_F                                         0x09
#define K_G                                         0x0A
#define K_H                                         0x0B
#define K_I                                         0x0C
#define K_J                                         0x0D
#define K_K                                         0x0E
#define K_L                                         0x0F
#define K_M                                         0x10
#define K_N                                         0x11
#define K_O                                         0x12
#define K_P                                         0x13
#define K_Q                                         0x14
#define K_R                                         0x15
#define K_S                                         0x16
#define K_T                                         0x17
#define K_U                                         0x18
#define K_V                                         0x19
#define K_W                                         0x1A
#define K_X                                         0x1B
#define K_Y                                         0x1C
#define K_Z                                         0x1D

/* Numbers */

#define K_1_EXCL                                    0x1E
#define K_2_AT                                      0x1F
#define K_3_HASH                                    0x20
#define K_4_DOLLAR                                  0x21
#define K_5_PERCENT                                 0x22
#define K_6_CARET                                   0x23
#define K_7_AMPERSAND                               0x24
#define K_8_ASTERISK                                0x25
#define K_9_LPAREN                                  0x26
#define K_0_RPAREN                                  0x27

#define K_1                                         K_ADD_MOD(K_MOD_EMPTY, K_1_EXCL)
#define K_2                                         K_ADD_MOD(K_MOD_EMPTY, K_2_AT)
#define K_3                                         K_ADD_MOD(K_MOD_EMPTY, K_3_HASH)
#define K_4                                         K_ADD_MOD(K_MOD_EMPTY, K_4_DOLLAR)
#define K_5                                         K_ADD_MOD(K_MOD_EMPTY, K_5_PERCENT)
#define K_6                                         K_ADD_MOD(K_MOD_EMPTY, K_6_CARET)
#define K_7                                         K_ADD_MOD(K_MOD_EMPTY, K_7_AMPERSAND)
#define K_8                                         K_ADD_MOD(K_MOD_EMPTY, K_8_ASTERISK)
#define K_9                                         K_ADD_MOD(K_MOD_EMPTY, K_9_LPAREN)
#define K_0                                         K_ADD_MOD(K_MOD_EMPTY, K_0_RPAREN)

#define K_EXCL                                      K_ADD_MOD(K_MOD_LSHIFT, K_1_EXCL)
#define K_AT                                        K_ADD_MOD(K_MOD_LSHIFT, K_2_AT)
#define K_HASH                                      K_ADD_MOD(K_MOD_LSHIFT, K_3_HASH)
#define K_DOLLAR                                    K_ADD_MOD(K_MOD_LSHIFT, K_4_DOLLAR)
#define K_PERCENT                                   K_ADD_MOD(K_MOD_LSHIFT, K_5_PERCENT)
#define K_CARET                                     K_ADD_MOD(K_MOD_LSHIFT, K_6_CARET)
#define K_AMPERSAND                                 K_ADD_MOD(K_MOD_LSHIFT, K_7_AMPERSAND)
#define K_ASTERISK                                  K_ADD_MOD(K_MOD_LSHIFT, K_8_ASTERISK)
#define K_LPAREN                                    K_ADD_MOD(K_MOD_LSHIFT, K_9_LPAREN)
#define K_RPAREN                                    K_ADD_MOD(K_MOD_LSHIFT, K_0_RPAREN)

/* Non-Letters */

#define K_ENTER                                     0x28
#define K_ESC                                       0x29
#define K_BACKSPACE                                 0x2A
#define K_TAB                                       0x2B
#define K_SPACE                                     0x2C

/* Symbols */

#define K_MINUS_UNDERSCORE                          0x2D
#define K_EQUAL_PLUS                                0x2E
#define K_LEFTBRACE_LEFTCURLY                       0x2F
#define K_RIGHTBRACE_RIGHTCURLY                     0x30
#define K_BACKSLASH_PIPE                            0x31
#define K_HASH_TILDE                                0x32
#define K_SEMICOLON_COLON                           0x33
#define K_APOSTROPHE_QUOTE                          0x34
#define K_GRAVE_TILDE                               0x35
#define K_COMMA_LESS                                0x36
#define K_DOT_GREATER                               0x37
#define K_SLASH_QUESTION                            0x38
#define K_CAPSLOCK                                  0x39

#define K_MINUS                                     K_ADD_MOD(K_MOD_EMPTY, K_MINUS_UNDERSCORE)
#define K_UNDERSCORE                                K_ADD_MOD(K_MOD_LSHIFT, K_MINUS_UNDERSCORE)
#define K_EQUAL                                     K_ADD_MOD(K_MOD_EMPTY, K_EQUAL_PLUS)
#define K_PLUS                                      K_ADD_MOD(K_MOD_LSHIFT, K_EQUAL_PLUS)
#define K_LEFTBRACE                                 K_ADD_MOD(K_MOD_EMPTY, K_LEFTBRACE_LEFTCURLY)
#define K_LEFTCURLY                                 K_ADD_MOD(K_MOD_LSHIFT, K_LEFTBRACE_LEFTCURLY)
#define K_RIGHTBRACE                                K_ADD_MOD(K_MOD_EMPTY, K_RIGHTBRACE_RIGHTCURLY)
#define K_RIGHTCURLY                                K_ADD_MOD(K_MOD_LSHIFT, K_RIGHTBRACE_RIGHTCURLY)
#define K_BACKSLASH                                 K_ADD_MOD(K_MOD_EMPTY, K_BACKSLASH_PIPE)
#define K_PIPE                                      K_ADD_MOD(K_MOD_LSHIFT, K_BACKSLASH_PIPE)
#define K_SEMICOLON                                 K_ADD_MOD(K_MOD_EMPTY, K_SEMICOLON_COLON)
#define K_COLON                                     K_ADD_MOD(K_MOD_LSHIFT, K_SEMICOLON_COLON)
#define K_APOSTROPHE                                K_ADD_MOD(K_MOD_EMPTY, K_APOSTROPHE_QUOTE)
#define K_QUOTE                                     K_ADD_MOD(K_MOD_LSHIFT, K_APOSTROPHE_QUOTE)
#define K_GRAVE                                     K_ADD_MOD(K_MOD_EMPTY, K_GRAVE_TILDE)
#define K_TILDE                                     K_ADD_MOD(K_MOD_LSHIFT, K_GRAVE_TILDE)
#define K_COMMA                                     K_ADD_MOD(K_MOD_EMPTY, K_COMMA_LESS)
#define K_LESS                                      K_ADD_MOD(K_MOD_LSHIFT, K_COMMA_LESS)
#define K_DOT                                       K_ADD_MOD(K_MOD_EMPTY, K_DOT_GREATER)
#define K_GREATER                                   K_ADD_MOD(K_MOD_LSHIFT, K_DOT_GREATER)
#define K_SLASH                                     K_ADD_MOD(K_MOD_EMPTY, K_SLASH_QUESTION)
#define K_QUESTION                                  K_ADD_MOD(K_MOD_LSHIFT, K_SLASH_QUESTION)

/* Function Keys */

#define K_F1                                        0x3A
#define K_F2                                        0x3B
#define K_F3                                        0x3C
#define K_F4                                        0x3D
#define K_F5                                        0x3E
#define K_F6                                        0x3F
#define K_F7                                        0x40
#define K_F8                                        0x41
#define K_F9                                        0x42
#define K_F10                                       0x43
#define K_F11                                       0x44
#define K_F12                                       0x45

#define LAYER                                       4
#define ROWS                                        4
#define COLS                                        10

static const uint16_t KEYMAP[LAYER][ROWS][COLS] = {
    /* Layer 1 */
    {
        {K_SEMICOLON,  K_COMMA,      K_DOT,       K_P,         K_Y,          K_F,          K_G,        K_C,          K_R,        K_L},
        {K_A,          K_O,          K_E,         K_U,         K_I,          K_D,          K_H,        K_T,          K_N,        K_S},
        {K_APOSTROPHE, K_Q,          K_J,         K_K,         K_X,          K_B,          K_M,        K_W,          K_V,        K_Z},
        {K_MOD_LCTRL_KEYMAP,  K_MOD_LSHIFT_KEYMAP, K_MOD_LGUI_KEYMAP,  0x00,        0x00,         K_TO_LAYER_1, K_SPACE,    K_TO_LAYER_2, 0x00,       0x00}
    },
    /* Layer 2 */
    {
        {K_1,          K_2,          K_3,          K_4,         K_5,          K_6,          K_7,        K_8,          K_9,          K_0},
        {K_PLUS,       K_LEFTBRACE,  K_LEFTCURLY,  K_LPAREN,    K_AMPERSAND,  K_EQUAL,      K_RPAREN,   K_RIGHTCURLY, K_RIGHTBRACE, K_ASTERISK},
        {K_HASH,       K_BACKSLASH,  K_UNDERSCORE, K_MINUS,     K_GRAVE,      K_PIPE,       K_EXCL,     K_PERCENT,    K_QUESTION,   K_CARET},
        {K_MOD_LCTRL_KEYMAP,  K_MOD_LSHIFT_KEYMAP, K_MOD_LGUI_KEYMAP,   0x00,        0x00,         K_TO_LAYER_1, K_SPACE,    K_TO_LAYER_2, 0x00,         0x00}
    }
};


// Since I use HID hex codes for the KEYMAP
// and I want to have a way to write to
// the OLED screens of the keyboard, I need
// a way to find the ASCII codes of each key
// I will index the array by
// [HID_hex][MOD_hex]
// MOD_hex is only SHIFT or NONE

static const char HID_TO_ASCII_MAP[][2] = {
    {0x61, 0x41}, // 0x04 (a, A)
    {0x62, 0x42}, // 0x05 (b, B)
    {0x63, 0x43}, // 0x06 (c, C)
    {0x64, 0x44}, // 0x07 (d, D)
    {0x65, 0x45}, // 0x08 (e, E)
    {0x66, 0x46}, // 0x09 (f, F)
    {0x67, 0x47}, // 0x0A (g, G)
    {0x68, 0x48}, // 0x0B (h, H)
    {0x69, 0x49}, // 0x0C (i, I)
    {0x6A, 0x4A}, // 0x0D (j, J)
    {0x6B, 0x4B}, // 0x0E (k, K)
    {0x6C, 0x4C}, // 0x0F (l, L)
    {0x6D, 0x4D}, // 0x10 (m, M)
    {0x6E, 0x4E}, // 0x11 (n, N)
    {0x6F, 0x4F}, // 0x12 (o, O)
    {0x70, 0x50}, // 0x13 (p, P)
    {0x71, 0x51}, // 0x14 (q, Q)
    {0x72, 0x52}, // 0x15 (r, R)
    {0x73, 0x53}, // 0x16 (s, S)
    {0x74, 0x54}, // 0x17 (t, T)
    {0x75, 0x55}, // 0x18 (u, U)
    {0x76, 0x56}, // 0x19 (v, V)
    {0x77, 0x57}, // 0x1A (w, W)
    {0x78, 0x58}, // 0x1B (x, X)
    {0x79, 0x59}, // 0x1C (y, Y)
    {0x7A, 0x5A}, // 0x1D (z, Z)
    {0x31, 0x21}, // 0x1E (1, !)
    {0x32, 0x40}, // 0x1F (2, @)
    {0x33, 0x23}, // 0x20 (3, #)
    {0x34, 0x24}, // 0x21 (4, $)
    {0x35, 0x25}, // 0x22 (5, %)
    {0x36, 0x5E}, // 0x23 (6, ^)
    {0x37, 0x26}, // 0x24 (7, &)
    {0x38, 0x2A}, // 0x25 (8, *)
    {0x39, 0x28}, // 0x26 (9, ()
    {0x30, 0x29}, // 0x27 (0, ))
    {0x00, 0x00}, // 0x28 Disable Enter
    {0x00, 0x00}, // 0x29 Disable ESC
    {0x00, 0x00}, // 0x2A Disable Backspace
    {0x00, 0x00}, // 0x2B Disable Tab
    {0x20, 0x20}, // 0x2C (Space, Space)
    {0x2D, 0x5F}, // 0x2D (-, _)
    {0x3D, 0x2B}, // 0x2E (=, +)
    {0x5B, 0x7B}, // 0x2F ([, {)
    {0x5D, 0x7D}, // 0x30 (], })
    {0x5C, 0x7C}, // 0x31 (\, |)
    {0x23, 0x7E}, // 0x32 (#, ~)
    {0x3B, 0x3A}, // 0x33 (;, :)
    {0x27, 0x22}, // 0x34 (', ")
    {0x60, 0x7E}, // 0x35 (`, ~)
    {0x2C, 0x3C}, // 0x36 (,, <)
    {0x2E, 0x3E}, // 0x37 (., >)
    {0x2F, 0x3F}, // 0x38 (/, ?)
};
