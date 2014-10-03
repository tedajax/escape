#include "input.h"

char SHIFTED_CHARS[256] = {
        0,     0,     0,     0,     0,     0,     0,     0, // 0x00 - 0x07
        0,     0,     0,     0,     0,     0,     0,     0, // 0x08 - 0x0F
        0,     0,     0,     0,     0,     0,     0,     0, // 0x10 - 0x17
        0,     0,     0,     0,     0,     0,     0,     0, // 0x18 - 0x1F
      ' ',   '!',  '\"',   '#',   '$',   '%',   '&',  '\'', // 0x20 - 0x27
      '(',   ')',   '*',   '+',   '<',   '_',   '>',   '?', // 0x28 - 0x2F
      ')',   '!',   '@',   '#',   '$',   '%',   '^',   '&', // 0x30 - 0x37
      '*',   '(',   ':',   ':',   '<',   '+',   '>',   '?', // 0x38 - 0x3F

      '@',   'a',   'b',   'c',   'd',   'e',   'f',   'g', // 0x40 - 0x47
      'h',   'i',   'j',   'k',   'l',   'm',   'n',   'o', // 0x48 - 0x4F
      'p',   'q',   'r',   's',   't',   'u',   'v',   'w', // 0x50 - 0x57
      'x',   'y',   'z',   '{',   '|',   '}',   '^',   '_', // 0x58 - 0x5F
      '~',   'A',   'B',   'C',   'D',   'E',   'F',   'G', // 0x60 - 0x67
      'H',   'I',   'J',   'K',   'L',   'M',   'N',   'O', // 0x68 - 0x6F
      'P',   'Q',   'R',   'S',   'T',   'U',   'V',   'W', // 0x70 - 0x77
      'X',   'Y',   'Z',   '{',   '|',   '}',   '~',     0, // 0x78 - 0x7F
};

char input_get_event_char(SDL_Event event) {
    if (event.type != SDL_KEYDOWN && event.type != SDL_KEYUP) {
        return 0;
    }

    SDL_Keycode symbol = event.key.keysym.sym;
    SDL_Keymode mod = event.key.keysym.mod;
    
    if (symbol == SDLK_UNKNOWN) {
        return 0;
    }

    if ((symbol & 0x40000000) > 0) {
        return 0;
    }

    bool shift = (mod & KMOD_SHIFT) > 0;
    bool caps = (mod & KMOD_CAPS) > 0;

    return 0;
}

unsigned char input_shifted_char(unsigned char c) {
    if (c > 127) { return c; }

    if (c >= 'a' && c <= 'z') {
        return c - 0x20;
    }

    return SHIFTED_CHARS[c];
}

unsigned char input_caps_char(unsigned char c) {

}