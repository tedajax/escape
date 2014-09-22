#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "types.h"
#include "containers.h"
#include "tables.h"
#include "parser.h"
#include "json.h"
#include "room.h"
#include "game.h"

int main(int argc, char *argv[]) {
    Game *game = game_new();

    int result = game_run(game, argc, argv);

    return result;
}