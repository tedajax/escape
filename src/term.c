#include "term.h"

int BASE = 1;

const char *COLORS[VIDEO_COLOR_COUNT] = {
    "\e[c,0]",
    "\e[c,1]",
    "\e[c,2]",
    "\e[c,3]",
    "\e[c,4]",
    "\e[c,5]",
    "\e[c,6]",
    "\e[c,7]",
    "\e[c,8]"
};

const char *BG_COLORS[VIDEO_COLOR_COUNT] = {
    "\e[b,0]",
    "\e[b,1]",
    "\e[b,2]",
    "\e[b,3]",
    "\e[b,4]",
    "\e[b,5]",
    "\e[b,6]",
    "\e[b,7]",
    "\e[b,8]"
};
