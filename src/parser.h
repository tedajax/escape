#ifndef _PARSER_H_
#define _PARSER_H_

#include "types.h"
#include "action.h"

bool parse_action(char *input, Action *dest);
Verb parse_verb(char *input);

#endif