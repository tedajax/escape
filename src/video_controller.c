#include "video_controller.h"

void videocontroller_set_mode(VideoController *self, u32 w, u32 h) {
    assert(self);

    self->width = w;
    self->height = h;
    self->size = self->width * self->height;

    if (self->data) {
        free(self->data);
    }

    self->data = calloc(self->size, sizeof(u32));

    self->dirty = true;
}

void videocontroller_open_font(VideoController *self, const char *filename, u32 px) {
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
    return true;
}

void videocontroller_poke(VideoController *self, u32 x, u32 y, u32 value) {
    assert(self);

    assert(x < self->width);
    assert(y < self->height);

    self->data[y * self->width + x] = value;
    self->dirty = true;
}

void videocontroller_update_glyphs(VideoController *self) {
    if (!self->dirty) { return; }

    for (u32 row = 0; row < self->height; ++row) {
        for (u32 col = 0; col < self->width; ++col) {
            u32 i = row * self->width + col;

            
        }
    }

    self->dirty = false;
}

void videocontroller_render_glyphs(VideoController *self) {
    for (u32 row = 0; row < self->height; ++row) {
        for (u32 col = 0; col < self->width; ++col) {
            u32 i = row * self->width + col;
            //todo render
        }
    }
}

void videocontroller_free(VideoController *self) {
    //todo
}