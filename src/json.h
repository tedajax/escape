#ifndef _JSON_H_
#define _JSON_H_

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "types.h"
#include "vector.h"
#include "hashtable.h"

//#define JSMN_STRICT
//#define JSMN_PARENT_LINKS

#include "jsmn.h"

#define MAX_JSON_TOKENS 256

typedef union json_object_t {
    Hashtable *object;
    Vector *array;
    String *string;
    f64 number;
    bool boolean;
} JsonObject;

char *json_load_file(const char *filename, char *buffer);
jsmntok_t *json_tokenize(char *js);

// tokens must be NULL terminated
Hashtable *json_build_from_tokens(jsmntok_t *tokens);

#endif