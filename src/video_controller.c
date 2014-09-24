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

    self->glyphs = (SDL_Texture **)calloc(self->size, sizeof(SDL_Texture *));

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

    return true;
}

void videocontroller_generate_glyph_table(VideoController *self) {
    //TODO clear existing glyph table

    char *glyphStr = calloc(2, sizeof(char));

    printf("Generating glyphs...\n");

    for (u32 col = 0; col < VIDEO_COLOR_COUNT; ++col) {
        printf("\tColor %d", col);
        u32 colorPacked = VIDEO_COLORS[col];
        u8 r = ((colorPacked & 0xFF0000) >> 16);
        u8 g = ((colorPacked & 0x00FF00) >> 8);
        u8 b = ((colorPacked & 0x0000FF) >> 0);
        SDL_Color color = { r, g, b };
        for (u32 c = 0; c < 256; ++c) {
            if (c % 32 == 0) {
                printf(".");
            }
            glyphStr[0] = (char)c;

            SDL_Surface *surface = TTF_RenderText_Solid(self->font, glyphStr, color);
            SDL_Texture *texture = SDL_CreateTextureFromSurface(self->renderer, surface);        
            SDL_FreeSurface(surface);

            self->glyphTable[col][c] = texture;
        }
        printf("\n");
    }

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

void videocontroller_dirty_range(VideoController *self, u32 start, u32 end) {
    self->dirty = true;
    Range *range = calloc(1, sizeof(Range));
    range->start = start;
    range->end = end;
    vector_add(self->dirtyRanges, range);
}

void videocontroller_update_glyphs(VideoController *self) {
    if (!self->dirty) { return; }

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

    self->dirty = false;
}

void videocontroller_update_range(VideoController *self, Range range) {
    for (u32 i = range.start; i <= range.end; ++i) {
        if (i >= self->size) { break; }

        u32 value = self->data[i];
        u32 colorIndex = ((value & 0xF00) >> 8);
        char glyph = (value & 0xFF);

        self->glyphs[i] = self->glyphTable[colorIndex][glyph];
    }
}

void videocontroller_render_glyphs(VideoController *self) {
    for (u32 row = 0; row < self->height; ++row) {
        for (u32 col = 0; col < self->width; ++col) {
            u32 i = row * self->width + col;
            SDL_Rect rect = { col * 10, row * 14 - 2, self->glyphWidth, self->glyphHeight };
            SDL_RenderCopy(self->renderer, self->glyphs[i], NULL, &rect);
        }
    }
}

void videocontroller_free(VideoController *self) {
    //todo
}