#ifndef _JSON_H_
#define _JSON_H_

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "types.h"
#include "vector.h"
#include "hashtable.h"

#define JSMN_STRICT
#define JSMN_PARENT_LINKS

#include "jsmn.h"

#define MAX_JSON_TOKENS 256

void json_load_file(const char *filename, char *buffer);
jsmntok_t *json_tokenize(char *js);

#endif