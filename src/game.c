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
    bool run = init_tables();

    while (run) {
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
        Verb verb = parse_verb(words);

        if (verb == VERB_LOOK) {
            room_look(self->currentRoom);
        }

        vector_free(words);
        string_free(inputStr);

        printf("Verb: %d\n", verb);

        if (verb == VERB_QUIT) {
            run = false;
        }
    }

    free(input);

    printf("Done...\n");

    return 0;
}