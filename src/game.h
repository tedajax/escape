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

const size_t MAX_INPUT_LENGTH;

typedef struct game_t {
    World *world;
    Room *currentRoom;
    bool run;
} Game;

Game *game_new();
void game_free(Game *self);

int game_run(Game *self, int argc, char *argv[]);
void game_do_action(Game *self, Action action);
void game_do_command(Game *self, Action action);

#endif