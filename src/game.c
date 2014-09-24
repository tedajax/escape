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

bool game_init(Game *self, int argc, char *argv[]) {
    srand(time(NULL));

    SET_COLOR(WHITE);

    world_load(self->world, "data/test.json");
    self->currentRoom = world_get_room(self->world, 1);
    
    self->run = init_tables();

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_PRINT_ERROR();
        return false;
    }

    if (TTF_Init() != 0) {
        printf("TTF_Init: %s\n", TTF_GetError());
        return false;
    }

    self->window = SDL_CreateWindow("Escape",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        800,
        600,
        SDL_WINDOW_SHOWN);

    if (!self->window) {
        SDL_PRINT_ERROR();
        return false;
    }

    self->renderer = SDL_CreateRenderer(self->window, -1, SDL_RENDERER_ACCELERATED);
    if (!self->renderer) {
        SDL_PRINT_ERROR();
        return false;
    }

    self->screen = SDL_GetWindowSurface(self->window);

    for (int ai = 0; ai < argc; ++ai) {
        if (strcmp(argv[ai], "--color") == 0) {
            g_enable_colors = 1;
        }
    }

    self->video = videocontroller_new(self->renderer);
    videocontroller_set_mode(self->video, 80, 43);
    videocontroller_open_font(self->video, "assets/modeseven.ttf", 14);
    videocontroller_update_glyphs(self->video);

    return true;
}

int game_run(Game *self, int argc, char *argv[]) {
    assert(self);

    if (!game_init(self, argc, argv)) {
        return 1;
    }

    room_look(self->currentRoom);

    char * input = calloc(MAX_INPUT_LENGTH, sizeof(char));
    
    u32 x = 0;
    u32 y = 0;
    u32 ci = 0;

    u32 lastTickCount = 0;
    u32 secondsTimer = 0;
    u32 frames = 0;
    while (self->run) {
        game_proc_events(self);
        // memset(input, 0, MAX_INPUT_LENGTH);

        // printf("> ");
        // fgets(input, MAX_INPUT_LENGTH, stdin);

        // size_t inputLen = strlen(input);
        // input[inputLen - 1] = '\0';

        // String *inputStr = S(input);

        // if (!parse_input_valid(inputStr)) {
        //     printf("I don't understand...\n");
        //     string_free(inputStr);
        //     continue;
        // }

        // Vector *words = parse_words(inputStr);
        // Action action = parse_action(words);
        
        // if (G_VERB_PATTERNS[action.verb] & VP_COMMAND) {
        //     game_do_command(self, action);
        // } else {
        //     game_do_action(self, action);
        // }

        // vector_free(words);
        // string_free(inputStr);

        u32 ticks = SDL_GetTicks();
        u32 delta = ticks - lastTickCount;
        lastTickCount = ticks;

        SDL_SetRenderDrawColor(self->renderer, 0, 0, 0, 255);
        SDL_RenderClear(self->renderer);
        
        u32 v = (ci + 64) + ((rand() % 7 + 1) << 8);
        videocontroller_poke(self->video, x, y, v);
        videocontroller_update_glyphs(self->video);
        videocontroller_render_glyphs(self->video);
        ++x;
        if (x >= self->video->width) {
            x = 0;
            ++y;
            if (y >= self->video->height) {
                videocontroller_form_feed(self->video);
                y = self->video->height - 1;
            }
        }
        ++ci; if (ci >= 26) { ci = 0; }

        SDL_RenderPresent(self->renderer);
        
        ++frames;
        secondsTimer += delta;

        if (secondsTimer >= 1000) {
            printf("FPS: %d\n", frames);
            secondsTimer = 0;
            frames = 0;
        }
    }

    free(input);

    printf("Done...\n");

    return 0;
}

void game_proc_events(Game *self) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                self->run = false;
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    self->run = false;
                } else if (event.key.keysym.sym == SDLK_p) {
                    u32 x = rand() % self->video->width;
                    u32 y = rand() % self->video->height;
                    u32 v = ((rand() % 26) + 65) + ((rand() % 8) << 8);
                    videocontroller_poke(self->video, x, y, v);
                    videocontroller_update_glyphs(self->video);
                } else if (event.key.keysym.sym == SDLK_RETURN) {
                    videocontroller_form_feed(self->video);
                    videocontroller_update_glyphs(self->video);
                }
                break;

            default: break;
        }
    }
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