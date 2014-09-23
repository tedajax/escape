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

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    self->window = SDL_CreateWindow("Escape",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        200,
        200,
        SDL_WINDOW_SHOWN);

    if (!self->window) {
        return 2;
    }

    self->renderer = SDL_CreateRenderer(self->window, -1, SDL_RENDERER_ACCELERATED);
    if (!self->renderer) {
        return 3;
    }

    self->screen = SDL_GetWindowSurface(self->window);

    for (int ai = 0; ai < argc; ++ai) {
        if (strcmp(argv[ai], "--color") == 0) {
            g_enable_colors = 1;
        }
    }

    SET_COLOR(WHITE);

    world_load(self->world, "data/test.json");
    self->currentRoom = world_get_room(self->world, 1);
    room_look(self->currentRoom);

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
        
        if (G_VERB_PATTERNS[action.verb] & VP_COMMAND) {
            game_do_command(self, action);
        } else {
            game_do_action(self, action);
        }

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
        default: break;

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

        case VERB_INVALID:
        case VERB_LAST:
            return;

        case VERB_GO:
        case VERB_TAKE:
        case VERB_USE:
        case VERB_HELP:
            //Todo
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

void game_do_command(Game *self, Action action) {
    switch (action.verb) {
        default: return;

        case VERB_QUIT:
            self->run = false;
            break;

        case VERB_HELP:
            printf("TODO: Help\n");
            break;

        case VERB_SAVE:
            printf("TODO: Save\n");
            break;

        case VERB_LOAD:
            printf("TODO: Load\n");
            break;

        case VERB_CONTINUE:
            printf("TODO: Continue\n");
            break;

        case VERB_COLOR:
            if (strcmp(action.cmdArg, "black") == 0) {
                SET_COLOR(BLACK);
                BASE = BLACK;
            } else if (strcmp(action.cmdArg, "red") == 0) {
                SET_COLOR(RED);
                BASE = RED;
            } else if (strcmp(action.cmdArg, "green") == 0) {
                SET_COLOR(GREEN);
                BASE = GREEN;
            } else if (strcmp(action.cmdArg, "yellow") == 0) {
                SET_COLOR(YELLOW);
                BASE = YELLOW;
            } else if (strcmp(action.cmdArg, "blue") == 0) {
                SET_COLOR(BLUE);
                BASE = BLUE;
            } else if (strcmp(action.cmdArg, "magenta") == 0) {
                SET_COLOR(MAGENTA);
                BASE = MAGENTA;
            } else if (strcmp(action.cmdArg, "cyan") == 0) {
                SET_COLOR(CYAN);
                BASE = CYAN;
            } else if (strcmp(action.cmdArg, "white") == 0) {
                SET_COLOR(WHITE);
                BASE = WHITE;
            } else {
                printf("Unknown color: %s\n", action.cmdArg);
            }
            break;
    }
}