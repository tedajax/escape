#include "input.h"

char SHIFTED_CHARS[128] = {
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
    SDL_Keymod mod = event.key.keysym.mod;
    
    if (symbol == SDLK_UNKNOWN) {
        return 0;
    }

    if ((symbol & 0x40000000) > 0) {
        printf("%d\n", symbol);
        char special = input_get_special_char(symbol);
        return special;
    }

    if (symbol == SDLK_RETURN) {
        return '\n';
    }

    bool shift = (mod & KMOD_SHIFT) > 0;
    bool caps = (mod & KMOD_CAPS) > 0;

    char result = 0;
    if (shift && !caps) {
        result = input_get_shifted_char(symbol);
    } else if (!shift && caps) {
        result = input_get_caps_char(symbol);
    } else if (shift && caps) {
        result = input_get_shifted_caps_char(symbol);
    } else {
        result = symbol;
    }

    return result;
}

char input_get_special_char(SDL_Keycode keycode) {
    switch(keycode) {
        case SDLK_KP_0: return '0';
        case SDLK_KP_1: return '1';
        case SDLK_KP_2: return '2';
        case SDLK_KP_3: return '3';
        case SDLK_KP_4: return '4';
        case SDLK_KP_5: return '5';
        case SDLK_KP_6: return '6';
        case SDLK_KP_7: return '7';
        case SDLK_KP_8: return '8';
        case SDLK_KP_9: return '9';
        case SDLK_KP_PERIOD: return '.';
        case SDLK_KP_DIVIDE: return '/';
        case SDLK_KP_MULTIPLY: return '*';
        case SDLK_KP_MINUS: return '-';
        case SDLK_KP_PLUS: return '+';
        default: return 0;
    }
}

char input_get_shifted_char(unsigned char c) {
    if (c > 127) { return c; }

    return SHIFTED_CHARS[c];
}

char input_get_caps_char(unsigned char c) {
    if (c >= 'a' && c <= 'z') {
        return c - 0x20;
    }

    return c;
}

char input_get_shifted_caps_char(unsigned char c) {
    if (c >= 'a' && c <= 'z') {
        return c;
    }

    return SHIFTED_CHARS[c];
}