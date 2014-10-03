#ifndef _INPUT_H_
#define _INPUT_H_

#include "types.h"

#include <SDL2/SDL.h>

#include "game.h"

char SHIFTED_CHARS[128];
char CAPS_CHARS[128];

char input_get_event_char(SDL_Event event);
unsigned char input_shifted_char(unsigned char c);
unsigned char input_caps_char(unsigned char c);

#endif