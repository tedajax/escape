#include "tables.h"

Verb *G_VERB_LIST = NULL;
Hashtable *G_WORD_TABLE_VERBS = NULL;
VerbPattern G_VERB_PATTERNS[VERB_LAST];

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

    ADD_VERB("look",    VERB_LOOK);
    ADD_VERB("examine", VERB_LOOK);

    ADD_VERB("east",    VERB_EAST);
    ADD_VERB("e",       VERB_EAST);
    ADD_VERB("west",    VERB_WEST);
    ADD_VERB("w",       VERB_WEST);
    ADD_VERB("north",   VERB_NORTH);
    ADD_VERB("n",       VERB_NORTH);
    ADD_VERB("south",   VERB_SOUTH);
    ADD_VERB("s",       VERB_SOUTH);

    ADD_VERB("go",      VERB_GO);
    ADD_VERB("g",       VERB_GO);
    ADD_VERB("move",    VERB_GO);
    ADD_VERB("m",       VERB_GO);

    ADD_VERB("take",    VERB_TAKE);
    ADD_VERB("use",     VERB_USE);

    ADD_VERB("quit",    VERB_QUIT);
    ADD_VERB("q",       VERB_QUIT);
    ADD_VERB("exit",    VERB_QUIT);    

    ADD_VERB("help",    VERB_HELP);

    G_VERB_PATTERNS[VERB_INVALID] = VP_NONE;
    G_VERB_PATTERNS[VERB_LAST] =    VP_NONE;

    G_VERB_PATTERNS[VERB_LOOK] =    VP_VERB | VP_VERB_SUBJECT | VP_VERB_SUBJECT_SUBJECT;
    G_VERB_PATTERNS[VERB_EAST] =    VP_VERB;
    G_VERB_PATTERNS[VERB_WEST] =    VP_VERB;
    G_VERB_PATTERNS[VERB_NORTH] =   VP_VERB;
    G_VERB_PATTERNS[VERB_SOUTH] =   VP_VERB;
    G_VERB_PATTERNS[VERB_UP] =      VP_VERB;
    G_VERB_PATTERNS[VERB_DOWN] =    VP_VERB;
    G_VERB_PATTERNS[VERB_GO] =      VP_VERB_SUBJECT;
    G_VERB_PATTERNS[VERB_TAKE] =    VP_VERB_SUBJECT | VP_VERB_SUBJECT_SUBJECT;
    G_VERB_PATTERNS[VERB_USE] =     VP_VERB_SUBJECT | VP_VERB_SUBJECT_SUBJECT;
    G_VERB_PATTERNS[VERB_QUIT] =    VP_VERB;
    G_VERB_PATTERNS[VERB_HELP] =    VP_VERB;

    return true;
}
