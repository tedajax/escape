#ifndef _PARSER_H_
#define _PARSER_H_

#include <stdlib.h>
#include <assert.h>

#include "types.h"
#include "containers.h"
#include "action.h"
#include "verb.h"
#include "tables.h"

// bool parse_action(char *input, Action *dest);
// Verb parse_verb(char *input);

bool parse_input_valid(String *input);
Vector *parse_words(String *input);
Verb parse_verb(Vector *words);

#endif