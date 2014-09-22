#ifndef _JSON_H_
#define _JSON_H_

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "types.h"
#include "vector.h"
#include "hashtable.h"

#include "jsmn.h"

#define DEFAULT_TOKEN_ALLOCATION 256

extern const u32 JSON_DEFAULT_HASHTABLE_BUCKETS;
extern const u32 JSON_DEFAULT_VECTOR_SIZE;

typedef enum json_token_type_e {
    JS_TOKEN_NULL,
    JS_TOKEN_OBJECT,
    JS_TOKEN_ARRAY,
    JS_TOKEN_STRING,
    JS_TOKEN_INTEGER,
    JS_TOKEN_NUMBER,
    JS_TOKEN_BOOLEAN
} JsonTokenType;

typedef struct json_token_t {
    JsonTokenType type;
    int id;
    int children;
    int parent;
    void *data;
} JsonToken;

#define JSON_VECTOR(token) ((Vector *)(token)->data)
#define JSON_HASHTABLE(token) ((Hashtable *)(token)->data)

const char *json_token_type_string(JsonTokenType type);

char *json_load_file(const char *filename);
jsmntok_t *json_tokenize(const char *js);
JsonToken *json_parse(const char *filename);

JsonToken *json_token_new(JsonTokenType type);
JsonToken *json_token_set_type(JsonToken *self, JsonTokenType type);
JsonToken *json_token_create(JsonToken *self, jsmntok_t token, int id, const char *js);
void json_token_free(JsonToken *self);
void json_token_free_void(void *self);
void json_token_print(JsonToken *self);

JsonToken *json_obj_get(JsonToken *object, const char *key);
int json_obj_get_int(JsonToken *object, const char *key);
f64 json_obj_get_f64(JsonToken *object, const char *key);
bool json_obj_get_bool(JsonToken *object, const char *key);
String *json_obj_get_string(JsonToken *object, const char *key);
JsonToken *json_obj_get_array(JsonToken *object, const char *key);
JsonToken *json_obj_get_object(JsonToken *object, const char *key);

JsonToken *json_arr_get(JsonToken *array, u32 index);
int json_arr_get_int(JsonToken *array, u32 index);
f64 json_arr_get_f64(JsonToken *array, u32 index);
bool json_arr_get_bool(JsonToken *array, u32 index);
String *json_arr_get_string(JsonToken *array, u32 index);
JsonToken *json_arr_get_array(JsonToken *array, u32 index);
JsonToken *json_arr_get_object(JsonToken *array, u32 index);

// tokens must be NULL terminated
JsonToken *json_build_from_tokens(jsmntok_t *tokens, const char *js);
JsonToken *json_build_tokens_length(jsmntok_t *tokens, size_t num, const char *js);

#endif