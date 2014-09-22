#ifndef _ACTION_H_
#define _ACTION_H_

#include "types.h"
#include "containers.h"
#include "verb.h"

/* Actions can take 3 forms:
 * 1) VERB
 * 2) VERB SUBJECT
 * 3) VERB SUBJECT with SECONDARY_SUBJECT
 */
typedef struct action_t {
    Verb verb;
    i32 subjId;
    i32 secSubjId;
    const char *cmdArg;
} Action;

Action *action_new();

#endif