#include "term.h"

int BASE = 7;
int g_enable_colors = 0;

const char *COLORS[COLOR_COUNT] = {
    "\033[1;30m",
    "\033[1;31m",
    "\033[1;32m",
    "\033[1;33m",
    "\033[1;34m",
    "\033[1;35m",
    "\033[1;36m",
    "\033[1;37m"
};

const char *BG_COLORS[COLOR_COUNT] = {
    "\033[1;40m",
    "\033[1;41m",
    "\033[1;42m",
    "\033[1;43m",
    "\033[1;44m",
    "\033[1;45m",
    "\033[1;46m",
    "\033[1;47m"
};
