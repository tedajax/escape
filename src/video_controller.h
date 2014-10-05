#ifndef _VIDEO_CONTROLLER_H_
#define _VIDEO_CONTROLLER_H_

#include <stdlib.h>
#include <assert.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "types.h"
#include "containers.h"
#include "colors.h"

typedef enum video_commands_e {
    VIDEO_CMD_NOOP,
    VIDEO_CMD_CLEAR,
    VIDEO_CMD_CHANGE_FG,
    VIDEO_CMD_CHANGE_BG,
    VIDEO_CMD_GOTOXY,
    VIDEO_CMD_BLINK_FG,
    VIDEO_CMD_BLINK_BG,
    VIDEO_CMD_LAST
} VideoCommands;

typedef struct range_t {
    u32 start;
    u32 end;
} Range;

// Glyph Flags - 16 bits
// 7654 3210
//
// 0 - Show flag, if 1 show the glyph fg
// 1 - Show background flag, if 1 show the glyph bg
// 2 - Blink flag, if 1 blink the foreground
// 3 - Blink BG flag, if 1 blink the background
// 4 - Bold
// 5 - Italics
// 6 - Underline
// 7 - Unused at this time

typedef enum glyph_flag_e {
    GLYPH_SHOW_FG       = 0x1,
    GLYPH_SHOW_BG       = 0x2,
    GLYPH_BLINK_FG      = 0x4,
    GLYPH_BLINK_BG      = 0x8,
    GLYPH_BOLD          = 0x10,
    GLYPH_ITALICS       = 0x20,
    GLYPH_UNDERLINE     = 0x40,
    GLYPH_UNUSED_7      = 0x80,
    GLYPH_UNUSED_8      = 0x100,
    GLYPH_UNUSED_9      = 0x200,
    GLYPH_UNUSED_A      = 0x400,
    GLYPH_UNUSED_B      = 0x800,
    GLYPH_UNUSED_C      = 0x1000,
    GLYPH_UNUSED_D      = 0x2000,
    GLYPH_UNUSED_E      = 0x4000,
    GLYPH_UNUSED_F      = 0x8000,
} GlyphFlags;

typedef struct glyph_t {
    SDL_Rect rect;
    SDL_Rect bgRect;
    GlyphFlags flags;
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
    bool hasParam1;
    bool hasParam2;
    VideoCommand *next;
} VideoCommand;

typedef struct video_text_state_t {
    u32 color;
    u32 bgColor;
    bool blink;
    bool bgBlink;
    bool bold;
    bool italics;
    bool underline;
} VideoTextState;

typedef struct video_controller_t {
    u32 size;
    u32 width;
    u32 height;
    Point cursor;
    u32 *data;
    Glyph *glyphs;
    SDL_Texture *glyphTexture;

    VideoTextState textState;

    u32 pxSize;
    int glyphWidth;
    int glyphHeight;
    TTF_Font *font;
    bool blinkFlag;
    u32 blinkTicks;
    u32 blinkDelay;

    bool cursorBlinkFlag;
    u32 cursorBlinkDelay;
    u32 cursorBlinkTicks;
    bool showCursor;

    bool inputOn;

    bool dirty;
    Vector *dirtyRanges;

    SDL_Renderer *renderer;

    SDL_mutex *dataMutex;
} VideoController;

VideoController *videoctl_new(SDL_Renderer *renderer);
void videoctl_set_mode(VideoController *self, u32 w, u32 h);
bool videoctl_open_font(VideoController *self, const char *filename, u32 px);
void videoctl_generate_glyph_table(VideoController *self);

GlyphFlags videoctl_get_glyph_flags(VideoController *self);

void videoctl_gotoxy(VideoController *self, u16 x, u16 y);
void videoctl_poke(VideoController *self, u32 x, u32 y, u32 value);
u32 videoctl_at_cursor(VideoController *self);
void videoctl_form_feed(VideoController *self);
void videoctl_carriage_return(VideoController *self);
void videoctl_new_line(VideoController *self);
void videoctl_backspace(VideoController *self);
void videoctl_delete(VideoController *self);
void videoctl_tab(VideoController *self);
void videoctl_print(VideoController *self, const char *string);
void videoctl_printf(VideoController *self, const char *format, ...);
void videoctl_printfv(VideoController *self, const char *format, va_list args);
void videoctl_putc(VideoController *self, char c);
void videoctl_step_cursor(VideoController *self);
void videoctl_step_cursorc(VideoController *self, u32 c);
void videoctl_set_color(VideoController *self, u32 colorIndex);
void videoctl_set_bgcolor(VideoController *self, u32 colorIndex);
void videoctl_clear(VideoController *self);
void videoctl_text_cmds(VideoController *self, VideoCommand *cmdList);
void videoctl_text_cmd(VideoController *self, VideoCommand cmd);
void videoctl_give_time(VideoController *self, u32 milliseconds);

void videoctl_handle_input(VideoController *self, u8 key);

void videoctl_dirty_range(VideoController *self, u32 start, u32 end);
void videoctl_update_glyphs(VideoController *self);
void videoctl_update_range(VideoController *self, Range range);
void videoctl_render_glyphs(VideoController *self);

void videoctl_color_test(VideoController *self);

u32 _videoctl_gen_data(VideoController *self, char c);
u32 _videoctl_gen_flags(VideoController *self);

void videoctl_free(VideoController *self);

VideoCommand *videocmd_parse(const char *str, u32 start, u32 end);
VideoCommand *videocmd_create(u32 argc, char **argv);
bool videocmd_bool(VideoCommand cmd, bool current);

struct param_parse_t {
    bool hasValue;
    i32 value;
};

VideoCommands _videocmd_parse_command(const char *cmd);
struct param_parse_t _videocmd_parse_param(const char *param);
void videocmd_free(VideoCommand *self);

#endif