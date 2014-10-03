#ifndef _INPUT_H_
#define _INPUT_H_

#include "types.h"

#include <SDL2/SDL.h>

#include "game.h"

char SHIFTED_CHARS[128];

char input_get_event_char(SDL_Event event);
char input_get_special_char(SDL_Keycode keycode);
char input_get_shifted_char(unsigned char c);
char input_get_caps_char(unsigned char c);
char input_get_shifted_caps_char(unsigned char c);

#endif