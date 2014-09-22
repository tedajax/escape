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

#ifdef COLORS_ENABLED
    #define SET_COLOR(color) printf("\033[1;%dm", color + FG_START)
    #define SET_BG_COLOR(color) printf("\033[1;%dm", color + BG_START)
    #define COLOR_LEN 7
#else
    #define SET_COLOR(color) ((void)0)
    #define SET_BG_COLOR(color) ((void)0)
    #define COLOR_LEN 0
#endif

#endif