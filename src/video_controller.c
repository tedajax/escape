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
        char c = ' ';
        if (i >= '!' && i <= '~') {
            c = (char)i;
        }
        glyphStr[i - 1] = c;
    }
    glyphStr[255] = '\0';

    printf("Generating glyphs...");

    SDL_Surface *textSurface = 
        SDL_CreateRGBSurface(0,
                             256 * self->glyphWidth,
                             VIDEO_COLOR_COUNT * self->glyphHeight,
                             32, 0, 0, 0, 0);

    for (u32 col = 0; col < VIDEO_COLOR_COUNT; ++col) {
        u32 colorPacked = VIDEO_COLORS[col];
        u8 r = ((colorPacked & 0xFF0000) >> 16);
        u8 g = ((colorPacked & 0x00FF00) >> 8);
        u8 b = ((colorPacked & 0x0000FF) >> 0);
        SDL_Color color = { r, g, b };
        SDL_Surface *surface = TTF_RenderText_Solid(self->font, glyphStr, color);
        SDL_Rect rect = {
            0,
            col * self->glyphHeight,
            256 * self->glyphWidth,
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

    u32 index = 0;
    while (string[index] != '\0') {
        videoctl_putc(self, string[index++]);
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
        str = realloc(str, length);
    }

    videoctl_print(self, str);
    free(str);
}

void videoctl_putc(VideoController *self, char c) {
    assert(self);

    u32 value = (self->color << 8) + ((u32)c);
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

void videoctl_clear(VideoController *self) {
    assert(self);
    for (u32 i = 0; i < self->size; ++i) {
        self->data[i] &= 0xFFFFFF00;
    }
    videoctl_dirty_range(self, 0, self->size - 1);
    videoctl_gotoxy(self, 0, 0);
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
        u32 colorIndex = ((value & 0xF00) >> 8);
        char glyph = (value & 0xFF);

        self->glyphs[i].rect.x = (int)(glyph - 1) * self->glyphWidth;
        self->glyphs[i].rect.y = colorIndex * self->glyphHeight;
        self->glyphs[i].rect.w = self->glyphWidth;
        self->glyphs[i].rect.h = self->glyphHeight;
    }
}

void videoctl_render_glyphs(VideoController *self) {
    for (u32 row = 0; row < self->height; ++row) {
        for (u32 col = 0; col < self->width; ++col) {
            u32 i = row * self->width + col;

            SDL_Rect rect = { 
                col * 10,
                row * 14 - 2,
                self->glyphWidth,
                self->glyphHeight
            };
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