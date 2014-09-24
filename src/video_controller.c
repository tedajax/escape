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

VideoController *videocontroller_new(SDL_Renderer *renderer) {
    VideoController *self = (VideoController *)calloc(1, sizeof(VideoController));
    self->renderer = renderer;
    self->dirtyRanges = vector_new(8, free);
    self->dataMutex = SDL_CreateMutex();
    return self;
}

void videocontroller_set_mode(VideoController *self, u32 w, u32 h) {
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

bool videocontroller_open_font(VideoController *self, const char *filename, u32 px) {
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

    videocontroller_generate_glyph_table(self);
    videocontroller_dirty_range(self, 0, self->size - 1);

    return true;
}

void videocontroller_generate_glyph_table(VideoController *self) {
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

    SDL_Surface *textSurface = SDL_CreateRGBSurface(0,
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
        SDL_Rect rect = { 0, col * self->glyphHeight, 256 * self->glyphWidth, self->glyphHeight };
        SDL_BlitSurface(surface, NULL, textSurface, &rect);
        SDL_FreeSurface(surface);
    }

    self->glyphTexture = SDL_CreateTextureFromSurface(self->renderer, textSurface);
    SDL_FreeSurface(textSurface);

    printf("done\n");

    free(glyphStr);
}

void videocontroller_poke(VideoController *self, u32 x, u32 y, u32 value) {
    assert(self);

    assert(x < self->width);
    assert(y < self->height);

    u32 pos = y * self->width + x;
    self->data[pos] = value;
    videocontroller_dirty_range(self, pos, pos);
}

void videocontroller_form_feed(VideoController *self) {
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

    videocontroller_dirty_range(self, 0, self->size - 1);
}

void videocontroller_dirty_range(VideoController *self, u32 start, u32 end) {
    self->dirty = true;
    Range *range = calloc(1, sizeof(Range));
    range->start = start;
    range->end = end;
    vector_add(self->dirtyRanges, range);
}

void videocontroller_update_glyphs(VideoController *self) {
    if (!self->dirty) { return; }

    if (SDL_LockMutex(self->dataMutex) == 0) {
        if (self->dirtyRanges->size > 0) {
            for (u32 i = 0; i < self->dirtyRanges->size; ++i) {
                Range *r = (Range *)vector_index(self->dirtyRanges, i);
                videocontroller_update_range(self, *r);
            }

            vector_clear(self->dirtyRanges);
        } else {
            Range everything = { 0, self->size - 1 };
            videocontroller_update_range(self, everything);
        }
        printf("updating glyphs\n");
        self->dirty = false;
        SDL_UnlockMutex(self->dataMutex);
    } else {
        printf("Unable to lock mutex\n");
    }
}

void videocontroller_update_range(VideoController *self, Range range) {
    for (u32 i = range.start; i <= range.end; ++i) {
        if (i >= self->size) { break; }

        u32 value = self->data[i];
        u32 colorIndex = ((value & 0xF00) >> 8);
        char glyph = (value & 0xFF);

        self->glyphs[i].rect.x = (int)glyph * self->glyphWidth;
        self->glyphs[i].rect.y = colorIndex * self->glyphHeight;
        self->glyphs[i].rect.w = self->glyphWidth;
        self->glyphs[i].rect.h = self->glyphHeight;
    }
}

void videocontroller_render_glyphs(VideoController *self) {
    for (u32 row = 0; row < self->height; ++row) {
        for (u32 col = 0; col < self->width; ++col) {
            u32 i = row * self->width + col;

            SDL_Rect rect = { col * 10, row * 14 - 2, self->glyphWidth, self->glyphHeight };
            // printf("%d\n", self->glyphs[5].rect.x);
            SDL_RenderCopy(self->renderer, self->glyphTexture, &self->glyphs[i].rect, &rect);
        }
    }
}

void videocontroller_free(VideoController *self) {
    //todo
}