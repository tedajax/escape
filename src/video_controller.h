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

typedef enum video_commands_e {
    VIDEO_CMD_NOOP,
    VIDEO_CMD_CLEAR,
    VIDEO_CMD_CHANGE_FG,
    VIDEO_CMD_CHANGE_BG,
    VIDEO_CMD_GOTOXY,
    VIDEO_CMD_LAST
} VideoCommands;

extern u32 VIDEO_COLORS[VIDEO_COLOR_COUNT];

typedef struct range_t {
    u32 start;
    u32 end;
} Range;

typedef struct glyph_t {
    SDL_Rect rect;
} Glyph;

typedef struct point_t {
    u16 x;
    u16 y;
} Point;

typedef struct video_cmd_t VideoCommand;

typedef struct video_cmd_t {
    VideoCommands command;
    i32 param1;
    i32 param2;
    VideoCommand *next;
} VideoCommand;

typedef struct video_controller_t {
    u32 size;
    u32 width;
    u32 height;
    Point cursor;
    u32 color;
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

VideoController *videoctl_new(SDL_Renderer *renderer);
void videoctl_set_mode(VideoController *self, u32 w, u32 h);
bool videoctl_open_font(VideoController *self, const char *filename, u32 px);
void videoctl_generate_glyph_table(VideoController *self);

void videoctl_gotoxy(VideoController *self, u16 x, u16 y);
void videoctl_poke(VideoController *self, u32 x, u32 y, u32 value);
void videoctl_form_feed(VideoController *self);
void videoctl_print(VideoController *self, const char *string);
void videoctl_printf(VideoController *self, const char *format, ...);
void videoctl_printfv(VideoController *self, const char *format, va_list args);
void videoctl_putc(VideoController *self, char c);
void videoctl_step_cursor(VideoController *self);
void videoctl_set_color(VideoController *self, u32 colorIndex);
void videoctl_clear(VideoController *self);
void videoctl_text_cmds(VideoController *self, VideoCommand *cmdList);
void videoctl_text_cmd(VideoController *self, VideoCommand cmd);

void videoctl_dirty_range(VideoController *self, u32 start, u32 end);
void videoctl_update_glyphs(VideoController *self);
void videoctl_update_range(VideoController *self, Range range);
void videoctl_render_glyphs(VideoController *self);

void videoctl_free(VideoController *self);

VideoCommand *videocmd_parse(const char *str, u32 start, u32 end);
VideoCommand *videocmd_create(u32 argc, char **argv);
void videocmd_free(VideoCommand *self);

#endif