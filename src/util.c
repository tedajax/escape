#include "util.h"

u32 strappend(char *str, u32 index, const char *append) {
    u32 appLen = strlen(append);
    for (u32 i = 0; i < appLen; ++i) {
        str[i + index] = append[i];
    }
    str[index + appLen] = '\0';
    return index + appLen;
}