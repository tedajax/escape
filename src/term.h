#ifndef _TERM_H_
#define _TERM_H_

#include "video_controller.h"

extern const char *COLORS[VIDEO_COLOR_COUNT];
extern const char *BG_COLORS[VIDEO_COLOR_COUNT];

extern int BASE;

#define RESET_COLOR() game_printf("\e[c,%d]", BASE)
#define SET_BASE(color) BASE = VIDEO_COLOR_##color;
#define SET_COLOR(color) game_printf("\e[c,%d]", VIDEO_COLOR_##color)
#define SET_BG_COLOR(color) game_printf("\e[b,%d]", VIDEO_COLOR_##color)

#endif