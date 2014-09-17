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

typedef enum verb_pattern_e {
    VP_NONE                    = 0x0,
    
    // e.g. North, Inventory, Quit
    VP_VERB                    = 0x1,

    // e.g. Go East, Use Axe, Take Scroll, Eat an Apple 
    VP_VERB_SUBJECT            = 0x2,

    // e.g. Use Scroll on Rat, Take Apple from Shelf
    VP_VERB_SUBJECT_SUBJECT    = 0x4
} VerbPattern;

bool verb_valid(Verb verb);

#endif