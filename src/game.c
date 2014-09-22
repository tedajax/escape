#include "game.h"

const size_t MAX_INPUT_LENGTH = 64;

Game *game_new() {
    Game *self = calloc(1, sizeof(Game));

    self->world = calloc(1, sizeof(World));
    self->currentRoom = NULL;

    return self;
}

void game_free(Game *self) {
    world_free(self->world);
    free(self);
}

int game_run(Game *self, int argc, char *argv[]) {
    assert(self);

    world_load(self->world, "data/test.json");
    self->currentRoom = world_get_room(self->world, 1);

    char * input = calloc(MAX_INPUT_LENGTH, sizeof(char));
    self->run = init_tables();

    while (self->run) {
        memset(input, 0, MAX_INPUT_LENGTH);

        printf("> ");
        fgets(input, MAX_INPUT_LENGTH, stdin);

        size_t inputLen = strlen(input);
        input[inputLen - 1] = '\0';

        String *inputStr = S(input);

        if (!parse_input_valid(inputStr)) {
            printf("I don't understand...\n");
            string_free(inputStr);
            continue;
        }

        Vector *words = parse_words(inputStr);
        Action action = parse_action(words);
        
        game_do_action(self, action);

        vector_free(words);
        string_free(inputStr);
    }

    free(input);

    printf("Done...\n");

    return 0;
}

void game_do_action(Game *self, Action action) {
    Verb verb = action.verb;

    i32 destId = 0;

    switch (verb) {
        case VERB_LOOK:
            room_look(self->currentRoom);
            break;

        case VERB_EAST:
        case VERB_WEST:
        case VERB_NORTH:
        case VERB_SOUTH:
        case VERB_UP:
        case VERB_DOWN:
            destId = self->currentRoom->exits[verb - VERB_EAST];
            if (destId == 0) { destId = -1; }
            break;

        case VERB_QUIT:
            self->run = false;
            return;
    }

    if (destId > 0) {
        Room *destination = world_get_room(self->world, destId);
        if (destination != NULL) {
            self->currentRoom = destination;
            room_look(self->currentRoom);
        }
    } else if (destId == -1) {
        printf("No exit in that direction.\n");
    }
}