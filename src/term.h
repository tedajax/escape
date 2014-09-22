#ifndef _TERM_H_
#define _TERM_H_

#define BLACK 0
#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define MAGENTA 5
#define CYAN 6
#define WHITE 7
#define COLOR_COUNT 8

#define FG_START 30
#define BG_START 40

extern const char *COLORS[COLOR_COUNT];
extern const char *BG_COLORS[COLOR_COUNT];

extern int BASE;
extern int g_enable_colors;

#define CHECK_COLORS_ON_START() if (g_enable_colors) {
#define CHECK_COLORS_ON_END() }

#define RESET_COLOR() CHECK_COLORS_ON_START() \
                      printf("\033[1;%dm", BASE + FG_START); \
                      CHECK_COLORS_ON_END()

#define SET_COLOR(color) CHECK_COLORS_ON_START() \
                         printf("\033[1;%dm", color + FG_START); \
                         CHECK_COLORS_ON_END()

#define SET_BG_COLOR(color) CHECK_COLORS_ON_START() \
                            printf("\033[1;%dm", color + BG_START); \
                            CHECK_COLORS_ON_END()

#define GET_COLOR(color) ((g_enable_colors) ? COLORS[color] : "")

#define COLOR_LEN ((g_enable_colors) ? 7 : 0)

#endif