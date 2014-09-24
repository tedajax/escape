#ifndef _VIDEO_CONTROLLER_H_
#define _VIDEO_CONTROLLER_H_

#include <stdlib.h>
#include <assert.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "types.h"

typedef enum video_colors_e {
    VIDEO_COLOR_BLACK = 0,
    VIDEO_COLOR_RED,
    VIDEO_COLOR_GREEN,
    VIDEO_COLOR_BLUE,
    VIDEO_COLOR_CYAN,
    VIDEO_COLOR_YELLOW,
    VIDEO_COLOR_MAGENTA,
    VIDEO_COLOR_GREY,
    VIDEO_COLOR_WHITE,
} VideoColors;

extern u32 VIDEO_COLORS[VIDEO_COLOR_COUNT];

typedef struct video_controller_t {
    u32 size;
    u32 width;
    u32 height;
    u32 *data;
    SDL_Texture *glyphs;
    bool dirty;

    u32 pxSize;
    u32 glyphWidth;
    u32 glyphHeight;
    TTF_Font *font;
} VideoController;

void videocontroller_set_mode(VideoController *self, u32 w, u32 h);
bool videocontroller_open_font(VideoController *self, const char *filename, u32 px);
void videocontroller_poke(VideoController *self, u32 x, u32 y, u32 value);
void videocontroller_update_glyphs(VideoController *self);
void videocontroller_render_glyphs(VideoController *self);
void videocontroller_free(VideoController *self);

#endif