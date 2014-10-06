#include "video_controller.h"

VideoController *videoctl_new(SDL_Renderer *renderer) {
    VideoController *self = (VideoController *)calloc(1, sizeof(VideoController));
    self->renderer = renderer;
    self->dirtyRanges = vector_new(8, free);
    self->dataMutex = SDL_CreateMutex();
    self->textState.color = VIDEO_COLOR_WHITE;
    self->textState.bgColor = VIDEO_COLOR_BLACK;
    self->blinkDelay = 500;
    self->showCursor = true;
    self->cursorBlinkFlag = false;
    self->cursorBlinkDelay = 250;
    self->prompt = "> ";
    self->inputMode = VIDEO_INPUT_FREE;
    self->textState.bgColor = VIDEO_COLOR_CLEAR;
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
    //TTF_SetFontStyle(self->font, TTF_STYLE_BOLD);
    SDL_Surface *surface = TTF_RenderText_Solid(self->font, "A", white);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(self->renderer, surface);

    SDL_QueryTexture(texture, NULL, NULL, &self->glyphWidth, &self->glyphHeight);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);

    printf("%d %d\n", self->glyphWidth, self->glyphHeight);

    videoctl_generate_glyph_table(self);
    videoctl_dirty_range(self, 0, self->size - 1);

    return true;
}

void videoctl_generate_glyph_table(VideoController *self) {
    //TODO clear existing glyph table

    char *glyphStr = calloc(129, sizeof(char));
    for (int i = 1; i < 128; ++i) {
        char c = (char)i;
        glyphStr[i - 1] = c;
    }

    printf("Generating glyphs...");

    u32 textSurfaceW = 256 * self->glyphWidth;
    u32 textSurfaceH = ((VIDEO_COLOR_COUNT / 2) + 1) * self->glyphHeight;
    SDL_Surface *textSurface =
        SDL_CreateRGBSurface(0,
                             textSurfaceW, textSurfaceH,
                             32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    SDL_Rect fullRect = {0, 0, textSurfaceW, textSurfaceH};
    SDL_FillRect(textSurface, &fullRect, 0x00000000);

    for (u32 col = 0; col < VIDEO_COLOR_COUNT; ++col) {
        u32 colorPacked = VIDEO_COLORS[col];
        u8 r = ((colorPacked & 0xFF0000) >> 16);
        u8 g = ((colorPacked & 0x00FF00) >> 8);
        u8 b = ((colorPacked & 0x0000FF) >> 0);
        SDL_Color color = { r, g, b, 255 };
        SDL_Surface *surface = TTF_RenderText_Blended(self->font, glyphStr, color);
        SDL_Rect rect = {
            (col % 2) * 1024,
            (col / 2) * self->glyphHeight,
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
            (VIDEO_COLOR_COUNT / 2) * self->glyphHeight,
            self->glyphWidth,
            self->glyphHeight
        };
        SDL_BlitSurface(surface, NULL, textSurface, &rect);
        SDL_FreeSurface(surface);
    }
    self->glyphTexture = SDL_CreateTextureFromSurface(self->renderer, textSurface);
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

u32 videoctl_at_cursor(VideoController *self) {
    assert(self);

    return self->data[self->cursor.y * self->width + self->cursor.x];
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

void videoctl_carriage_return(VideoController *self) {
    self->cursor.x = 0;
}

void videoctl_new_line(VideoController *self) {
    self->cursor.x = 0;
    ++self->cursor.y;
    if (self->cursor.y >= self->height) {
        self->cursor.y = self->height - 1;
        videoctl_form_feed(self);
    }
}

void videoctl_backspace(VideoController *self) {
    bool canBackspace = (self->inputMode == INPUT_MODE_FREE);

    if (self->inputMode == INPUT_MODE_PROMPT) {
        if (self->cursor.y == self->promptStart.y) {
            if (self->cursor.x > self->promptStart.x) {
                canBackspace = true;
            }
        } else {
            canBackspace = true;
        }
    }

    if (!canBackspace) {
        return;
    }

    if (self->cursor.x > 0) {
        --self->cursor.x;
    } else {
        if (self->cursor.y > 0) {
            --self->cursor.y;
            self->cursor.x = self->width - 1;
        }
    }

    u32 value = _videoctl_gen_data(self, 0);
    videoctl_poke(self, self->cursor.x, self->cursor.y, value);
}

void videoctl_delete(VideoController *self) {
    for (u32 i = self->cursor.x; i < (self->width - 1); ++i) {
        u32 index = self->cursor.y * self->width + i;
        self->data[index] = self->data[index + 1];
    }
    self->data[self->cursor.y * self->width + (self->width - 1)] = 0;
    videoctl_dirty_range(self, self->cursor.y * self->width, self->cursor.y * self->width + (self->width - 1));
}

void videoctl_tab(VideoController *self) {
    videoctl_step_cursorc(self, 4);
}

void videoctl_cursor_up(VideoController *self) {
    switch (self->inputMode) {
        case VIDEO_INPUT_FREE:
            if (self->cursor.y > 0) {
                --self->cursor.y;
            }
            break;

        case VIDEO_INPUT_PROMPT:
            if (self->cursor.y > self->promptStart.y) {
                --self->cursor.y;
                if (self->cursor.y == self->promptStart.y) {
                    if (self->cursor.x < self->promptStart.x) {
                        self->cursor.x = self->promptStart.x;
                    }
                }
            }
            break

        default: break;
    }
}

void videoctl_cursor_down(VideoController *self) {
    switch (self->inputMode) {
        case VIDEO_INPUT_FREE:
            if (self->cursor.y < self->promptEnd.y) {
                ++self->cursor.y;
                if (self->cursor.y == self->promptEnd.y) {
                    if (self->cursor.x > self->promptEnd.x) {
                        self->cursor.x = promptEnd.x;
                    }
                }
            }
            break;

        case VIDEO_INPUT_PROMPT:

            break

        default: break;
    }
}

void videoctl_cursor_left(VideoController *self) {
    switch (self->inputMode) {
        case VIDEO_INPUT_FREE:
            if (self->cursor.x > 0) {
                --self->cursor.x;
            }
            break;

        case VIDEO_INPUT_PROMPT:
            if (self->cursor.x > 0) {
                if (self->cursor.y == self->promptStart.y) {
                    if (self->cursor.x > self->promptStart.x) {
                        --self->cursor.x;
                    }
                } else {
                    --self->cursor.x;
                }
            } else {
                if (self->cursor.y > self->promptStart.y) {
                    self->cursor.x = self->width - 1;
                    --self->cursor.y;
                }
            }
            break

        default: break;
    }    
}

void videoctl_cursor_right(VideoController *self) {
    switch (self->inputMode) {
        case VIDEO_INPUT_FREE:
            if (self->cursor.x < self->width - 1) {
                ++self->cursor.x;
            }
            break;

        case VIDEO_INPUT_PROMPT:
            if (self->cursor.x < self->width - 1) {
                if (self->cursor.y == self->prompEnd.y) {
                    if (self->cursor.x < self->promptEnd.x) {
                        ++self->curosr.x;
                    }
                } else {
                    ++self->cursor.x;
                }
            } else {
                if (self->cursor.y < self->promptEnd.y) {
                    self->cursor.x = 0;
                    ++self->cursor.y;
                }
            }
            break

        default: break;
    }    
}

void videoctl_print(VideoController *self, const char *string) {
    assert(self);

    size_t len = strlen(string);
    u32 index = 0;
    u32 start;
    u32 end;
    VideoCommand *cmdList = NULL;
    while (string[index] != '\0') {
        switch (string[index]) {
            case '\b':
                videoctl_backspace(self);
                ++index;
                break;

            case '\t':
                videoctl_tab(self);
                ++index;
                break;

            case '\r':
                videoctl_carriage_return(self);
                ++index;
                break;

            case '\n':
                videoctl_new_line(self);
                ++index;
                break;

            case 127:
                videoctl_delete(self);
                ++index;
                break;

            case '\e':
                start = index + 1;
                end = start;
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

                cmdList = videocmd_parse(string, start + 1, end);
                videoctl_text_cmds(self, cmdList);
                videocmd_free(cmdList);

                index = end + 1;
                break;

            default:
                videoctl_putc(self, string[index++]);
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
    size_t length = 64;

    char *str = calloc(length, sizeof(char));

    //there's a bug in here somewhere
    while (vsnprintf(str, length, format, args) > length) {
        length <<= 1;
        str = realloc(str, length * sizeof(char));
    }

    videoctl_print(self, str);
    free(str);
}

void videoctl_putc(VideoController *self, char c) {
    assert(self);

    u32 value = _videoctl_gen_data(self, c);
    
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

void videoctl_step_cursorc(VideoController *self, u32 c) {
    for (u32 i = 0; i < c; ++i) {
        videoctl_step_cursor(self);
    }
}

void videoctl_set_color(VideoController *self, u32 colorIndex) {
    assert(self);
    assert(colorIndex < VIDEO_COLOR_COUNT);
    self->textState.color = colorIndex;
}

void videoctl_set_bgcolor(VideoController *self, u32 colorIndex) {
    assert(self);
    assert(colorIndex < VIDEO_COLOR_COUNT);
    self->textState.bgColor = colorIndex;
}

void videoctl_clear(VideoController *self) {
    assert(self);
    for (u32 i = 0; i < self->size; ++i) {
        self->data[i] &= 0xFF000000;
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

        case VIDEO_CMD_BLINK_FG:
            self->textState.blink = videocmd_bool(cmd, self->textState.blink);
            break;

        case VIDEO_CMD_BLINK_BG:
            self->textState.bgBlink = videocmd_bool(cmd, self->textState.bgBlink);
            break;

        default: break;
    }
}

void videoctl_give_time(VideoController *self, u32 milliseconds) {
    self->blinkTicks += milliseconds;
    while (self->blinkTicks > self->blinkDelay) {
        self->blinkTicks -= self->blinkDelay;
        self->blinkFlag = !self->blinkFlag;
        videoctl_dirty_range(self, 0, self->size - 1);
    }

    self->cursorBlinkTicks += milliseconds;
    while (self->cursorBlinkTicks > self->cursorBlinkDelay) {
        self->cursorBlinkTicks -= self->cursorBlinkDelay;
        self->cursorBlinkFlag = !self->cursorBlinkFlag;
    }
}

void videoctl_handle_input(VideoController *self, SDL_Event event) {
    assert(self);

    if (self->inputMode == VIDEO_INPUT_NONE) {
        return;
    }

    if ((event.key.keysym.sym & 0x40000000) > 0) {
        videoctl_handle_special_input(self, event);
    }

    char c = input_get_event_char(event);
    if (c) {
        videoctl_printf(self, "%c", c);
    }

    videoctl_handle_control_input(self, event);
}

void videoctl_handle_control_input(VideoController *self, SDL_Event event) {
    assert(self);

    if ((event.key.keysym.mod & KMOD_CTRL) == 0) {
        return;
    }

    SDL_Keycode key = event.key.keysym.sym;
    bool shift = (event.key.keysym.mod & KMOD_SHIFT);

    if (!shift) {
        switch (key) {
            case SDLK_h: videoctl_cursor_left(self); break;
            case SDLK_l: videoctl_cursor_right(self); break;
            case SDLK_k: videoctl_cursor_up(self); break;
            case SDLK_j: videoctl_cursor_down(self); break;

            case SDLK_b: self->textState.blink = !self->textState.blink; break;

            case SDLK_MINUS:
                if (self->textState.color > 0) {
                    --self->textState.color;
                } else {
                    self->textState.color = VIDEO_COLOR_COUNT - 1;
                }
                break;

            case SDLK_EQUALS:
                ++self->textState.color;
                if (self->textState.color >= VIDEO_COLOR_COUNT) {
                    self->textState.color = 0;
                }
                break;

            case SDLK_c: videoctl_clear(self); break;
            case SDLK_t: videoctl_color_test(self); break;
            
            default: break;
        }
    } else {
        switch (key) {
            case SDLK_b: self->textState.bgBlink = !self->textState.bgBlink; break;
            
            case SDLK_MINUS:
                if (self->textState.bgColor > 0) {
                    --self->textState.bgColor;
                } else {
                    self->textState.bgColor = VIDEO_COLOR_COUNT - 1;
                }
                break;

            case SDLK_EQUALS:
                ++self->textState.bgColor;
                if (self->textState.bgColor >= VIDEO_COLOR_COUNT) {
                    self->textState.bgColor = 0;
                }
                break;

            default: break;
        }
    }
}

void videoctl_handle_special_input(VideoController *self, SDL_Event event) {
    SDL_Keycode key = event.key.keysym.sym;
    switch (key) {
        case SDLK_UP: videoctl_cursor_up(self); break;
        case SDLK_DOWN: videoctl_cursor_down(self); break;
        case SDLK_LEFT: videoctl_cursor_left(self); break;
        case SDLK_RIGHT: videoctl_cursor_right(self); break;
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
    i32 transparentX = VIDEO_COLOR_COUNT * self->glyphWidth;
    i32 transparentY = VIDEO_COLOR_COUNT * self->glyphHeight;
    for (u32 i = range.start; i <= range.end; ++i) {
        if (i >= self->size) { break; }

        u32 value = self->data[i];
        u32 colorIndex = ((value & 0xFF00) >> 8);
        u32 bgColorIndex = ((value & 0xFF0000) >> 16);
        GlyphFlags flags = ((value & 0xFFFF0000) >> 24);
        char glyph = (value & 0xFF);

        self->glyphs[i].flags = flags;

        int colorColumn = (colorIndex % 2) * 1024;
        self->glyphs[i].rect.x = (int)(glyph - 1) * self->glyphWidth + colorColumn;
        self->glyphs[i].rect.y = (colorIndex / 2) * self->glyphHeight;
        self->glyphs[i].rect.w = self->glyphWidth;
        self->glyphs[i].rect.h = self->glyphHeight;

        self->glyphs[i].bgRect.x = bgColorIndex * self->glyphWidth;
        self->glyphs[i].bgRect.y = (VIDEO_COLOR_COUNT / 2) * self->glyphHeight;
        self->glyphs[i].bgRect.w = self->glyphWidth;
        self->glyphs[i].bgRect.h = self->glyphHeight;

        bool showFG = (flags & GLYPH_SHOW_FG);
        bool showBG = (flags & GLYPH_SHOW_BG);
        bool blinkFG = (flags & GLYPH_BLINK_FG);
        bool blinkBG = (flags & GLYPH_BLINK_BG);

        if (!showFG || (blinkFG && self->blinkFlag)) {
            self->glyphs[i].rect.x = transparentX;
            self->glyphs[i].rect.y = transparentY;
        }

        if (!showBG || (blinkBG && self->blinkFlag)) {
            self->glyphs[i].bgRect.x = transparentX;
            self->glyphs[i].bgRect.y = transparentY;
        }
    }
}

void videoctl_render_glyphs(VideoController *self) {
    for (u32 row = 0; row < self->height; ++row) {
        for (u32 col = 0; col < self->width; ++col) {
            u32 i = row * self->width + col;

            SDL_Rect rect = {
                col * self->glyphWidth * 2,
                row * self->glyphHeight * 2,
                self->glyphWidth * 2,
                self->glyphHeight * 2
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

    if (!self->showCursor) {
        return;
    }

    SDL_Rect cursorRect = {
        self->cursor.x * self->glyphWidth * 2,
        self->cursor.y * self->glyphHeight * 2,
        self->glyphWidth * 2,
        self->glyphHeight * 2
    };

    if (!self->cursorBlinkFlag) {
        SDL_SetRenderDrawColor(self->renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(self->renderer, &cursorRect);
    }
}

void videoctl_color_test(VideoController *self) {
    const u32 REPEAT_COUNT = 1;
    u32 color = 0;
    for (u32 i = 0; i < REPEAT_COUNT * 256; ++i) {
        videoctl_poke(self, self->cursor.x, self->cursor.y, (0b11 << 24) + (color << 16) + (VIDEO_COLOR_BLACK << 8));
        ++color;
        if (color >= VIDEO_COLOR_COUNT) {
            color = 0;
        }
        videoctl_step_cursor(self);
    }

    videoctl_dirty_range(self, 0, self->size - 1);
}

u32 _videoctl_gen_data(VideoController *self, char c) {
    u32 value =  _videoctl_gen_flags(self);
    if (self->textState.bgColor != VIDEO_COLOR_CLEAR) {
        value += (self->textState.bgColor << 16);
    } else {
        value += (videoctl_at_cursor(self) & 0x00FF0000) << 16;
    }
    value     += (self->textState.color << 8);
    value     += (u32)c;
    return value;
}

u32 _videoctl_gen_flags(VideoController *self) {
    u32 flags = 0b00000011;
    if (self->textState.blink) {
        flags += 0b100;
    }
    if (self->textState.bgBlink) {
        flags += 0b1000;
    }

    return (flags << 24);
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
        char *cmd = calloc(strlen(cmdStrs[i]) + 1, sizeof(char));
        strcpy(cmd, cmdStrs[i]);

        u32 paramCount = 0;
        u32 paramLimit = 3;
        const u32 paramLengthLimit = 32;
        char **paramStrs = calloc(paramLimit, sizeof(char *));
        char *pParamStr = strtok(cmd, argDelim);
        while (pParamStr) {
            paramStrs[paramCount] = calloc(paramLengthLimit, sizeof(char));
            strcpy(paramStrs[paramCount], pParamStr);
            ++paramCount;
            pParamStr = strtok(NULL, argDelim);
        }

        if (paramCount == 0) {
            free(paramStrs);
            free(cmd);
            continue;
        }

        VideoCommand *newCmd = videocmd_create(paramCount, paramStrs);

        for (u32 j = 0; j < paramCount; ++j) {
            free(paramStrs[j]);
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

    VideoCommands command = _videocmd_parse_command(argv[0]);
    if (command == VIDEO_CMD_NOOP) {
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

    struct param_parse_t parse1 = _videocmd_parse_param(param1Str);
    struct param_parse_t parse2 = _videocmd_parse_param(param2Str);

    result->hasParam1 = parse1.hasValue;
    result->hasParam2 = parse2.hasValue;
    result->param1 = parse1.value;
    result->param2 = parse2.value;

    return result;
}

VideoCommands _videocmd_parse_command(const char *cmd) {
    VideoCommands command = VIDEO_CMD_NOOP;

    if (strcmp(cmd, "clr") == 0) {
        command = VIDEO_CMD_CLEAR;
    } else if (strcmp(cmd, "c") == 0) {
        command = VIDEO_CMD_CHANGE_FG;
    } else if (strcmp(cmd, "b") == 0) {
        command = VIDEO_CMD_CHANGE_BG;
    } else if (strcmp(cmd, "goto") == 0) {
        command = VIDEO_CMD_GOTOXY;
    } else if (strcmp(cmd, "blink") == 0) {
        command = VIDEO_CMD_BLINK_FG;
    } else if (strcmp(cmd, "blinkbg") == 0) {
        command = VIDEO_CMD_BLINK_BG;
    }

    return command;
}

bool videocmd_bool(VideoCommand cmd, bool current) {
    if (cmd.hasParam1) {
        if (cmd.param1 > 0) {
            return true;
        } else {
            return false;
        }
    } else {
        return !current;
    }
}

struct param_parse_t _videocmd_parse_param(const char *param) {
    struct param_parse_t result = { false, 0 };

    if (!param) {
        return result;
    }

    result.hasValue = true;
    i32 value = 0;
    if (strcmp(param, "on") == 0 || strcmp(param, "true") == 0) {
        value = 1;
    } else if (strcmp(param, "off") == 0 || strcmp(param, "false") == 0) {
        value = 0;
    } else if (strcmp(param, "toggle") == 0) {
        value = -1;
    } else {
        value = atoi(param);
    }
    result.value = value;

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