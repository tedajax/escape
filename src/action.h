#ifndef _ACTION_H_
#define _ACTION_H_

#include "types.h"

typedef enum verb_e {
    VERB_INVALID,
    
    VERB_LOOK,
    
    // Directions
    VERB_EAST,
    VERB_WEST,
    VERB_NORTH,
    VERB_SOUTH,
    VERB_UP,
    VERB_DOWN,

    VERB_TAKE,
    VERB_USE
} Verb;

/* Actions can take 3 forms:
 * 1) VERB
 * 2) VERB SUBJECT
 * 3) VERB SUBJECT with SECONDARY_SUBJECT
 */
typedef struct action_t {
    Verb verb;
    i32 subjId;
    i32 secSubjId;
} Action;

Action *action_new();

#endif