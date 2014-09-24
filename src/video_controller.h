#ifndef _VIDEO_CONTROLLER_H_
#define _VIDEO_CONTROLLER_H_

#include <stdlib.h>
#include <assert.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "types.h"
#include "containers.h"

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
    VIDEO_COLOR_COUNT
} VideoColor;

extern u32 VIDEO_COLORS[VIDEO_COLOR_COUNT];

typedef struct range_t {
    u32 start;
    u32 end;
} Range;

typedef struct glyph_t {
    SDL_Rect rect;
} Glyph;

typedef struct video_controller_t {
    u32 size;
    u32 width;
    u32 height;
    u32 *data;
    Glyph *glyphs;
    SDL_Texture *glyphTexture;

    u32 pxSize;
    int glyphWidth;
    int glyphHeight;
    TTF_Font *font;
    
    bool dirty;
    Vector *dirtyRanges;

    SDL_Renderer *renderer;

    SDL_mutex *dataMutex;
} VideoController;

VideoController *videocontroller_new(SDL_Renderer *renderer);
void videocontroller_set_mode(VideoController *self, u32 w, u32 h);
bool videocontroller_open_font(VideoController *self, const char *filename, u32 px);
void videocontroller_generate_glyph_table(VideoController *self);
void videocontroller_poke(VideoController *self, u32 x, u32 y, u32 value);
void videocontroller_form_feed(VideoController *self);
void videocontroller_dirty_range(VideoController *self, u32 start, u32 end);
void videocontroller_update_glyphs(VideoController *self);
void videocontroller_update_range(VideoController *self, Range range);
void videocontroller_render_glyphs(VideoController *self);
void videocontroller_free(VideoController *self);

#endif