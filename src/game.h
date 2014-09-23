#ifndef _GAME_H_
#define _GAME_H_

#include <stdlib.h>

#include "types.h"
#include "containers.h"
#include "json.h"
#include "room.h"
#include "world.h"
#include "parser.h"
#include "tables.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define SDL_PRINT_ERROR() printf("SDL_Error: %s\n", SDL_GetError())

const size_t MAX_INPUT_LENGTH;

typedef struct game_t {
    World *world;
    Room *currentRoom;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *screen;
    bool run;
} Game;

Game *game_new();
void game_free(Game *self);

bool game_init(Game *self, int argc, char *argv[]);
int game_run(Game *self, int argc, char *argv[]);
void game_do_action(Game *self, Action action);
void game_do_command(Game *self, Action action);

#endif