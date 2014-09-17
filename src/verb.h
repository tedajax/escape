#ifndef _VERB_H_
#define _VERB_H_

#include <stdbool.h>

typedef enum verb_e {
    VERB_INVALID = 0,
    
    VERB_LOOK,
    
    // Directions
    VERB_EAST,
    VERB_WEST,
    VERB_NORTH,
    VERB_SOUTH,
    VERB_UP,
    VERB_DOWN,
    VERB_GO,  // Special case, identifies subject as direction to travel in.

    VERB_TAKE,
    VERB_USE,

    // Meta stuff
    VERB_QUIT,
    VERB_HELP,

    VERB_LAST
} Verb;

bool verb_valid(Verb verb);

#endif