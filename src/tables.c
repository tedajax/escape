#include "tables.h"

Verb *G_VERB_LIST = NULL;
Hashtable *G_WORD_TABLE_VERBS = NULL;

bool init_tables() {
    bool result = init_verb_table();
    return result;
}

bool init_verb_table() {
    G_VERB_LIST = calloc(VERB_LAST, sizeof(Verb));

    for (int i = 0; i < VERB_LAST; ++i) {
        G_VERB_LIST[i] = (Verb)i;
    }

    G_WORD_TABLE_VERBS = hashtable_new(64, NULL);

    if (!G_WORD_TABLE_VERBS) {
        return false;
    }

    ADD_VERB("look",        VERB_LOOK);
    ADD_VERB("examine",     VERB_LOOK);

    ADD_VERB("east",        VERB_EAST);
    ADD_VERB("e",           VERB_EAST);
    ADD_VERB("west",        VERB_WEST);
    ADD_VERB("w",           VERB_WEST);
    ADD_VERB("north",       VERB_NORTH);
    ADD_VERB("n",           VERB_NORTH);
    ADD_VERB("south",       VERB_SOUTH);
    ADD_VERB("s",           VERB_SOUTH);

    ADD_VERB("go",          VERB_GO);
    ADD_VERB("g",           VERB_GO);
    ADD_VERB("move",        VERB_GO);
    ADD_VERB("m",           VERB_GO);

    ADD_VERB("take",        VERB_TAKE);
    ADD_VERB("use",         VERB_USE);

    ADD_VERB("quit",        VERB_QUIT);
    ADD_VERB("q",           VERB_QUIT);
    ADD_VERB("exit",        VERB_QUIT);    

    ADD_VERB("help",        VERB_HELP);

    return true;
}
