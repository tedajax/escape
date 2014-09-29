#include "video_controller.h"

u32 VIDEO_COLORS[VIDEO_COLOR_COUNT] = {
    0x000000,
    0xff0000,
    0x00ff00,
    0x0000ff,
    0x00ffff,
    0xffff00,
    0xff00ff,
    0x7f7f7f,
    0xffffff
};

VideoController *videoctl_new(SDL_Renderer *renderer) {
    VideoController *self = (VideoController *)calloc(1, sizeof(VideoController));
    self->renderer = renderer;
    self->dirtyRanges = vector_new(8, free);
    self->dataMutex = SDL_CreateMutex();
    self->color = VIDEO_COLOR_WHITE;
    self->bgColor = VIDEO_COLOR_GREEN;
    return self;
}

void videoctl_set_mode(VideoController *self, u32 w, u32 h) {
    assert(self);

    self->width = w;
    self->height = h;
    self->size = self->width * self->height;

    if (self->data) {
        free(self->data);
    }

    self->data = calloc(self->size, sizeof(u32));
    memset(self->data, 0x800, self->size);

    self->glyphs = (Glyph *)calloc(self->size, sizeof(Glyph));

    self->dirty = true;
}

bool videoctl_open_font(VideoController *self, const char *filename, u32 px) {
    assert(self);

    TTF_Font *font = TTF_OpenFont(filename, px);
    if (!font) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        return false;
    }

    if (self->font) {
        //todo free font
    }

    self->font = font;

    SDL_Color white = { 255, 255, 255 };
    SDL_Surface *surface = TTF_RenderText_Solid(self->font, "A", white);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(self->renderer, surface);

    SDL_QueryTexture(texture, NULL, NULL, &self->glyphWidth, &self->glyphHeight);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);

    videoctl_generate_glyph_table(self);
    videoctl_dirty_range(self, 0, self->size - 1);

    return true;
}

void videoctl_generate_glyph_table(VideoController *self) {
    //TODO clear existing glyph table

    char *glyphStr = calloc(256, sizeof(char));
    for (int i = 1; i < 256; ++i) {
        char c = (char)i;
        // if (i >= '!' && i <= '~') {
        //     c = (char)i;
        // }
        glyphStr[i - 1] = c;
    }
    glyphStr[255] = '\0';

    printf("Generating glyphs...");

    u32 textSurfaceW = 256 * self->glyphWidth;
    u32 textSurfaceH = (VIDEO_COLOR_COUNT + 1) * self->glyphHeight;
    SDL_Surface *textSurface = 
        SDL_CreateRGBSurface(0,
                             textSurfaceW, textSurfaceH,
                             32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    SDL_Rect fullRect = {0, 0, textSurfaceW, textSurfaceH};
    SDL_FillRect(textSurface, &fullRect, 0x00000000);
    IMG_SavePNG(textSurface, "a.png");

    for (u32 col = 0; col < VIDEO_COLOR_COUNT; ++col) {
        u32 colorPacked = VIDEO_COLORS[col];
        u8 r = ((colorPacked & 0xFF0000) >> 16);
        u8 g = ((colorPacked & 0x00FF00) >> 8);
        u8 b = ((colorPacked & 0x0000FF) >> 0);
        SDL_Color color = { r, g, b, 255 };
        SDL_Surface *surface = TTF_RenderText_Blended(self->font, glyphStr, color);
        SDL_Rect rect = {
            0,
            col * self->glyphHeight,
            256 * self->glyphWidth,
            self->glyphHeight
        };
        SDL_BlitSurface(surface, NULL, textSurface, &rect);
        SDL_FreeSurface(surface);
    }

    for (u32 col = 0; col < VIDEO_COLOR_COUNT; ++col) {
        u32 colorPacked = VIDEO_COLORS[col];
        u8 r = ((colorPacked & 0xFF0000) >> 16);
        u8 g = ((colorPacked & 0x00FF00) >> 8);
        u8 b = ((colorPacked & 0x0000FF) >> 0);

        SDL_Surface *surface = SDL_CreateRGBSurface(0,
                                                    self->glyphWidth,
                                                    self->glyphHeight,
                                                    32, 0, 0, 0, 0);
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, r, g, b));
        SDL_Rect rect = {
            col * self->glyphWidth,
            VIDEO_COLOR_COUNT * self->glyphHeight,
            self->glyphWidth,
            self->glyphHeight
        };
        SDL_BlitSurface(surface, NULL, textSurface, &rect);
        SDL_FreeSurface(surface);
    }

    self->glyphTexture = SDL_CreateTextureFromSurface(self->renderer, textSurface);
    IMG_SavePNG(textSurface, "out.png");
    SDL_FreeSurface(textSurface);

    printf("done\n");

    free(glyphStr);
}

void videoctl_gotoxy(VideoController *self, u16 x, u16 y) {
    assert(self);
    assert(x < self->width);
    assert(y < self->height);

    self->cursor.x = x;
    self->cursor.y = y;
}

void videoctl_poke(VideoController *self, u32 x, u32 y, u32 value) {
    assert(self);

    assert(x < self->width);
    assert(y < self->height);

    u32 pos = y * self->width + x;
    self->data[pos] = value;
    videoctl_dirty_range(self, pos, pos);
}

void videoctl_form_feed(VideoController *self) {
    assert(self);

    for (u32 row = 0; row < self->height; ++row) {
        for (u32 col = 0; col < self->width; ++col) {
            if (row < self->height - 1) {
                u32 pos = row * self->width + col;
                u32 next = (row + 1) * self->width + col;
                self->data[pos] = self->data[next];
            } else {
                u32 pos = row * self->width + col;
                self->data[pos] = 0;
            }
        }
    }

    videoctl_dirty_range(self, 0, self->size - 1);
}

void videoctl_print(VideoController *self, const char *string) {
    assert(self);

    size_t len = strlen(string);
    u32 index = 0;
    while (string[index] != '\0') {
        if (string[index] != '\e') {
            videoctl_putc(self, string[index++]);
        } else {
            u32 start = index + 1;
            u32 end = start;
            if (start >= len || string[start] != '[') {
                ++index;
                continue;
            }

            while (string[end] != ']') {
                ++end;
                if (end >= len) {
                    ++index;
                    continue;
                }
            }

            VideoCommand *cmdList = videocmd_parse(string, start + 1, end);
            videoctl_text_cmds(self, cmdList);
            videocmd_free(cmdList);

            index = end + 1;
        }
    }
}

void videoctl_printf(VideoController *self, const char *format, ...) {
    va_list args;
    va_start(args, format);
    videoctl_printfv(self, format, args);
    va_end(args);
}

void videoctl_printfv(VideoController *self, const char *format, va_list args) {
    size_t length = strlen(format);
    length <<= 1;

    char *str = calloc(length, sizeof(char));
    while (vsnprintf(str, length, format, args) > length) {
        length <<= 1;
        str = realloc(str, length * sizeof(char));
    }

    videoctl_print(self, str);
    free(str);
}

void videoctl_putc(VideoController *self, char c) {
    assert(self);

    u32 value = (self->bgColor << 16) + (self->color << 8) + ((u32)c);
    videoctl_poke(self, self->cursor.x, self->cursor.y, value);
    videoctl_step_cursor(self);
}

void videoctl_step_cursor(VideoController *self) {
    assert(self);

    ++self->cursor.x;
    if (self->cursor.x >= self->width) {
        self->cursor.x = 0;
        ++self->cursor.y;
        if (self->cursor.y >= self->height) {
            self->cursor.y = self->height - 1;
            videoctl_form_feed(self);
        }
    }
}

void videoctl_set_color(VideoController *self, u32 colorIndex) {
    assert(self);
    assert(colorIndex < VIDEO_COLOR_COUNT);
    self->color = colorIndex;
}

void videoctl_set_bgcolor(VideoController *self, u32 colorIndex) {
    assert(self);
    assert(colorIndex < VIDEO_COLOR_COUNT);
    self->bgColor = colorIndex;
}

void videoctl_clear(VideoController *self) {
    assert(self);
    for (u32 i = 0; i < self->size; ++i) {
        self->data[i] &= 0xFFFFFF00;
    }
    videoctl_dirty_range(self, 0, self->size - 1);
    videoctl_gotoxy(self, 0, 0);
}

void videoctl_text_cmds(VideoController *self, VideoCommand *cmdList) {
    assert(self);

    VideoCommand *current = cmdList;
    while (current) {
        videoctl_text_cmd(self, *current);
        current = current->next;
    }
}

void videoctl_text_cmd(VideoController *self, VideoCommand cmd) {
    switch (cmd.command) {
        case VIDEO_CMD_CLEAR:
            videoctl_clear(self);
            break;

        case VIDEO_CMD_CHANGE_FG:
            videoctl_set_color(self, cmd.param1);
            break;

        case VIDEO_CMD_CHANGE_BG:
            videoctl_set_bgcolor(self, cmd.param1);
            break;

        case VIDEO_CMD_GOTOXY:
            videoctl_gotoxy(self, cmd.param1, cmd.param2);
            break;

        default: break;
    }
}

void videoctl_dirty_range(VideoController *self, u32 start, u32 end) {
    self->dirty = true;
    Range *range = calloc(1, sizeof(Range));
    range->start = start;
    range->end = end;
    vector_add(self->dirtyRanges, range);
}

void videoctl_update_glyphs(VideoController *self) {
    if (!self->dirty) { return; }

    if (SDL_LockMutex(self->dataMutex) == 0) {
        if (self->dirtyRanges->size > 0) {
            for (u32 i = 0; i < self->dirtyRanges->size; ++i) {
                Range *r = (Range *)vector_index(self->dirtyRanges, i);
                videoctl_update_range(self, *r);
            }

            vector_clear(self->dirtyRanges);
        } else {
            Range everything = { 0, self->size - 1 };
            videoctl_update_range(self, everything);
        }
        self->dirty = false;
        SDL_UnlockMutex(self->dataMutex);
    } else {
        printf("Unable to lock mutex\n");
    }
}

void videoctl_update_range(VideoController *self, Range range) {
    for (u32 i = range.start; i <= range.end; ++i) {
        if (i >= self->size) { break; }

        u32 value = self->data[i];
        u32 colorIndex = ((value & 0xFF00) >> 8);
        u32 bgColorIndex = ((value & 0xFF0000) >> 16);
        char glyph = (value & 0xFF);

        self->glyphs[i].rect.x = (int)(glyph - 1) * self->glyphWidth;
        self->glyphs[i].rect.y = colorIndex * self->glyphHeight;
        self->glyphs[i].rect.w = self->glyphWidth;
        self->glyphs[i].rect.h = self->glyphHeight;

        self->glyphs[i].bgRect.x = bgColorIndex * self->glyphWidth;
        self->glyphs[i].bgRect.y = (VIDEO_COLOR_COUNT + 1) * self->glyphHeight;
        self->glyphs[i].bgRect.w = self->glyphWidth;
        self->glyphs[i].bgRect.h = self->glyphHeight;
    }
}

void videoctl_render_glyphs(VideoController *self) {
    for (u32 row = 0; row < self->height; ++row) {
        for (u32 col = 0; col < self->width; ++col) {
            u32 i = row * self->width + col;

            SDL_Rect rect = { 
                col * self->glyphWidth,
                row * 14 - 2,
                self->glyphWidth,
                self->glyphHeight
            };

            SDL_RenderCopy(self->renderer,
                           self->glyphTexture,
                           &self->glyphs[i].bgRect,
                           &rect);

            SDL_RenderCopy(self->renderer,
                           self->glyphTexture,
                           &self->glyphs[i].rect,
                           &rect);
        }
    }
}

void videoctl_free(VideoController *self) {
    //todo
}

VideoCommand *videocmd_parse(const char *str, u32 start, u32 end) {
    if (end - start <= 0 || end - start > 4000000000) {
        return NULL;
    }

    VideoCommand *root = calloc(1, sizeof(VideoCommand));
    char *string = calloc(end - start + 1, sizeof(char));
    strncpy(string, &str[start], end - start);

    const char *cmdDelim = ";";
    const u32 cmdLengthLimit = 32;
    u32 cmdLimit = 16;
    char **cmdStrs = calloc(cmdLimit, sizeof(char *));
    u32 cmdCount = 0;
    char *pCmdStr = strtok(string, cmdDelim);
    while (pCmdStr) {
        if (cmdCount >= cmdLimit - 1) {
            cmdLimit <<= 1;
            cmdStrs = realloc(cmdStrs, cmdLimit * sizeof(char));
        }
        cmdStrs[cmdCount] = calloc(cmdLengthLimit, sizeof(char));
        strncpy(cmdStrs[cmdCount], pCmdStr, cmdLengthLimit);
        ++cmdCount;
        pCmdStr = strtok(NULL, cmdDelim);
    }

    free(string);

    if (cmdCount == 0) {
        free(root);
        free(cmdStrs);
        return NULL;
    }

    const char *argDelim = ",";
    VideoCommand *current = root;
    for (u32 i = 0; i < cmdCount; ++i) {
        char *cmd = calloc(strlen(cmdStrs[i]), sizeof(char));
        strcpy(cmd, cmdStrs[i]);

        u32 paramCount = 0;
        u32 paramLimit = 3;
        const u32 paramLengthLimit = 8;
        char **paramStrs = calloc(paramLimit, sizeof(char *));
        char *pParamStr = strtok(cmd, argDelim);
        while (pParamStr) {
            paramStrs[paramCount] = calloc(paramLengthLimit, sizeof(char));
            strncpy(paramStrs[paramCount], pParamStr, paramLengthLimit);
            ++paramCount;
            pParamStr = strtok(NULL, argDelim);
        }

        if (paramCount == 0) {
            free(paramStrs);
            free(cmd);
            continue;
        }

        VideoCommand *newCmd = videocmd_create(paramCount, paramStrs);
        
        for (u32 i = 0; i < paramCount; ++i) {
            free(paramStrs[i]);
        }
        free(paramStrs);

        free(cmd);
        free(cmdStrs[i]);

        if (newCmd) {
            current->next = newCmd;
            current = current->next;
        }
    }    

    free(cmdStrs);

    return root;
}

VideoCommand *videocmd_create(u32 argc, char **argv) {
    if (argc == 0) {
        return NULL;
    }

    char *cmd = argv[0];
    VideoCommands command = VIDEO_CMD_NOOP;

    if (strcmp(cmd, "clr") == 0) {
        command = VIDEO_CMD_CLEAR;
    } else if (strcmp(cmd, "c") == 0) {
        command = VIDEO_CMD_CHANGE_FG;
    } else if (strcmp(cmd, "b") == 0) {
        command = VIDEO_CMD_CHANGE_BG;
    } else if (strcmp(cmd, "goto") == 0) {
        command = VIDEO_CMD_GOTOXY;
    } else {
        return NULL;
    }

    VideoCommand *result = calloc(1, sizeof(VideoCommand));
    result->command = command;

    char *param1Str = NULL;
    char *param2Str = NULL;

    if (argc >= 3) {
        param1Str = argv[1];
        param2Str = argv[2];
    } else if (argc >= 2) {
        param1Str = argv[1];
    }

    i32 param1 = 0;
    i32 param2 = 0;

    if (param1Str) {
        param1 = atoi(param1Str);
    }

    if (param2Str) {
        param2 = atoi(param2Str);
    }

    result->param1 = param1;
    result->param2 = param2;

    return result;
}

void videocmd_free(VideoCommand *self) {
    VideoCommand *next = self;
    while (next) {
        VideoCommand *current = next;
        next = current->next;
        free(current);
    }
}