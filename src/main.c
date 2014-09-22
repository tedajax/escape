#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "game.h"

int main(int argc, char *argv[]) {
    Game *game = game_new();
    int result = game_run(game, argc, argv);
    game_free(game);
    return result;
}