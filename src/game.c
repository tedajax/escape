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

    u32 flags = IMG_INIT_JPG | IMG_INIT_PNG;
    if (IMG_Init(flags) != flags) {
        printf("IMG_Init: %s\n", IMG_GetError());
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

    self->video = videoctl_new(self->renderer);
    videoctl_set_mode(self->video, 100, 43);
    videoctl_open_font(self->video, "assets/dejavumono.ttf", 14);
    videoctl_update_glyphs(self->video);

    self->updateDelay = 100;

    return true;
}

int game_run(Game *self, int argc, char *argv[]) {
    assert(self);

    if (!game_init(self, argc, argv)) {
        return 1;
    }

    self->updateThread = SDL_CreateThread(game_update, "update_thread", (void *)self);
    if (self->updateThread == NULL) {
        printf("SDL_CreateThread: %s\n", SDL_GetError());
    }

    room_look(self->currentRoom);

    char * input = calloc(MAX_INPUT_LENGTH, sizeof(char));
    
    u32 lastTickCount = 0;
    u32 secondsTimer = 0;
    u32 frames = 0;
    while (self->run) {
        game_proc_events(self);

        u32 ticks = SDL_GetTicks();
        u32 delta = ticks - lastTickCount;
        lastTickCount = ticks;

        game_render(self);

        ++frames;
        secondsTimer += delta;

        if (secondsTimer >= 1000) {
            printf("FPS: %d\n", frames);
            secondsTimer = 0;
            frames = 0;
        }
    }

    SDL_WaitThread(self->updateThread, NULL);

    free(input);

    printf("Done...\n");

    return 0;
}

int game_update(void *pself) {
    Game *self = (Game *)pself;

    u32 ticks = 0;
    u32 lastTicks = 0;

    while (self->run) {
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
        
        //u32 v = ((rand() % 26) + 64) + ((rand() % 7 + 1) << 8);
        //videoctl_poke(self->video, x, y, v);

        videoctl_printf(self->video, "\e[c,8]Testing \e[c,5]colors \e[c,2]wow ", 5+10, 3.14f);
        videoctl_update_glyphs(self->video);
        
        ticks = SDL_GetTicks();
        videoctl_give_time(self->video, ticks - lastTicks);
        lastTicks = ticks;

        SDL_Delay(self->updateDelay);
    }

    return 1;
}

void game_render(Game *self) {
    SDL_SetRenderDrawColor(self->renderer, 0, 0, 0, 255);
    SDL_RenderClear(self->renderer);

    videoctl_render_glyphs(self->video);

    SDL_RenderPresent(self->renderer);
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
                    videoctl_poke(self->video, x, y, v);
                    videoctl_update_glyphs(self->video);
                } else if (event.key.keysym.sym == SDLK_RETURN) {
                    videoctl_form_feed(self->video);
                    videoctl_update_glyphs(self->video);
                } else if (event.key.keysym.sym == SDLK_MINUS) {
                    if (self->updateDelay > 10) {
                        self->updateDelay -= 10;
                    } else if (self->updateDelay > 1) {
                        --self->updateDelay;
                    } else {
                        self->updateDelay = 0;
                    }

                    printf("Update Delay: %d\n", self->updateDelay);
                } else if (event.key.keysym.sym == SDLK_EQUALS) {
                    if (self->updateDelay >= 10) {
                        self->updateDelay += 10;
                    } else {
                        ++self->updateDelay;
                    }

                    printf("Update Delay: %d\n", self->updateDelay);
                } else if (event.key.keysym.sym == SDLK_c) {
                    videoctl_clear(self->video);
                } else if (event.key.keysym.sym == SDLK_x) {
                    videoctl_set_color(self->video, rand() % 7 + 1);
                    videoctl_set_bgcolor(self->video, rand() % 7 + 1);
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