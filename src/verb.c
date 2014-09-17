#include "verb.h"

bool verb_valid(Verb verb) {
    return (verb > VERB_INVALID && verb < VERB_LAST);
}