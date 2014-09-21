#include "json.h"

const u32 JSON_DEFAULT_HASHTABLE_BUCKETS = 8;
const u32 JSON_DEFAULT_VECTOR_SIZE = 8;

const char *json_token_type_string(JsonTokenType type) {
    switch (type) {
        default:
        case JS_TOKEN_NULL: return "null";
        case JS_TOKEN_OBJECT: return "object";
        case JS_TOKEN_ARRAY: return "array";
        case JS_TOKEN_STRING: return "string";
        case JS_TOKEN_NUMBER: return "number";
        case JS_TOKEN_INTEGER: return "integer";
        case JS_TOKEN_BOOLEAN: return "boolean";
    }
}

char *json_load_file(const char *filename) {
    FILE *fp;
    long fileSize;
    char *buffer = NULL;

    fp = fopen(filename, "rb");
    ASSERT(fp, "Could not open file: ", filename);

    fseek(fp, 0L, SEEK_END);
    fileSize = ftell(fp);
    rewind(fp);

    buffer = (char *)calloc(fileSize+1, sizeof(char));
    if (!buffer) {
        fclose(fp);
        ASSERT(false, "memory allocation failure");
    }

    if (fread(buffer, fileSize, 1, fp) != 1) {
        fclose(fp);
        free(buffer);
        ASSERT(false, "file read failure");
    }

    fclose(fp);

    return buffer;
}

jsmntok_t *json_tokenize(const char *js) {
    ASSERT(js, "NULL json data passed to json_tokenize.");

    jsmn_parser parser;
    jsmn_init(&parser);

    unsigned int n = DEFAULT_TOKEN_ALLOCATION;
    jsmntok_t *tokens = calloc(n, sizeof(jsmntok_t));
    ASSERT(tokens, "allocation failure");

    int ret = jsmn_parse(&parser, js, strlen(js), tokens, n);

    while (ret == JSMN_ERROR_NOMEM) {
        n = n * 2 + 1;
        tokens = realloc(tokens, sizeof(jsmntok_t) * n);
        ASSERT(tokens, "reallocation failure");
        ret = jsmn_parse(&parser, js, strlen(js), tokens, n);
    }

    ASSERT(ret != JSMN_ERROR_INVAL, "jsmn_parse: invalid JSON string.");
    ASSERT(ret != JSMN_ERROR_PART, "jsmn_parse: truncated JSON string.");
    
    return tokens;
}

JsonToken *json_token_new(JsonTokenType type) {
    JsonToken *self = calloc(1, sizeof(JsonToken));

    self->id = -1;
    self->children = -1;
    self->parent = -1;
    self->data = NULL;
    
    json_token_set_type(self, type);

    return self;
}

JsonToken *json_token_set_type(JsonToken *self, JsonTokenType type) {
    assert(self);

    //TODO: freeing shit up
    assert(self->data == NULL);

    self->type = type;

    switch (self->type) {
        default:
        case JS_TOKEN_NULL:
            self->data = NULL;
            break;

        case JS_TOKEN_OBJECT:
            self->data = (void *)hashtable_new(JSON_DEFAULT_HASHTABLE_BUCKETS,
                                               json_token_free_void);
            break;

        case JS_TOKEN_ARRAY:
            self->data = (void *)vector_new(JSON_DEFAULT_VECTOR_SIZE,
                                            json_token_free_void);
            break;

        case JS_TOKEN_STRING:
            self->data = NULL;
            break;

        case JS_TOKEN_NUMBER:
            self->data = calloc(1, sizeof(f64));
            (*(f64 *)self->data) = 0.0;
            break;

        case JS_TOKEN_INTEGER:
            self->data = calloc(1, sizeof(i64));
            (*(i64 *)self->data) = 0;
            break;

        case JS_TOKEN_BOOLEAN:
            self->data = calloc(1, sizeof(bool));
            (*(bool *)self->data) = false;
            break;
    }

    return self;
}

JsonToken *json_token_create(JsonToken *self, jsmntok_t token, int id, const char *js) {
    JsonTokenType type = JS_TOKEN_NULL;

    char c = 0;

    switch (token.type) {
        case JSMN_OBJECT:
            type = JS_TOKEN_OBJECT;
            break;

        case JSMN_ARRAY:
            type = JS_TOKEN_ARRAY;
            break;

        case JSMN_STRING:
            type = JS_TOKEN_STRING;
            break;

        case JSMN_PRIMITIVE:
            c = js[token.start];

            if (c == '-' || (c >= '0' && c <= '9')) {
                type = JS_TOKEN_INTEGER;
                for (size_t i = token.start; i < token.end; ++i) {
                    if (js[i] == '.') {
                        type = JS_TOKEN_NUMBER;
                        break;
                    }
                }
            } else if (c == 't' || c == 'f') {
                type = JS_TOKEN_BOOLEAN;
            }

            break;
        default: break;
    }

    if (!self) {
        self = json_token_new(type);
    } else {
        json_token_set_type(self, type);
    }

    size_t dataLen = token.end - token.start;
    char *dataStr = calloc(dataLen, sizeof(char));
    char *dataEnd;

    memcpy(dataStr, &js[token.start], dataLen);

    switch (self->type) {
        case JS_TOKEN_BOOLEAN:
            if (js[token.start] == 't') {
                (*(bool *)self->data) = true;
            }
            break;

        case JS_TOKEN_NUMBER:
            (*(f64 *)self->data) = strtod(dataStr, &dataEnd);
            break;

        case JS_TOKEN_INTEGER:
            (*(i64 *)self->data) = atoi(dataStr);
            break;

        case JS_TOKEN_STRING:
            self->data = string_reserve(dataLen + 1);
            memcpy(((String *)self->data)->characters, dataStr, dataLen);
            break;

        default: break;
    }

    self->id = id;
    self->parent = token.parent;
    self->children = token.size;

    return self;
}

void json_token_free(JsonToken *self) {
    // TODO: this
}

void json_token_free_void(void *self) {
    json_token_free((JsonToken *)self);
}

void json_token_print(JsonToken *self) {
    printf("{ type: %s, id: %d, children: %d, parent: %d, value: ",
        json_token_type_string(self->type),
        self->id,
        self->children,
        self->parent);

    switch (self->type) {
        case JS_TOKEN_NULL: printf("null"); break;
        case JS_TOKEN_STRING: printf("%s", ((String *)self->data)->characters); break;
        case JS_TOKEN_NUMBER: printf("%f", (f64 *)self->data); break;
        case JS_TOKEN_BOOLEAN: printf("%s", (*(bool *)self->data) ? "true" : "false"); break;
        case JS_TOKEN_ARRAY:
        case JS_TOKEN_OBJECT: printf("todo"); break;
        default: printf("impossibru"); break;
    }

    printf(" }\n");
}

JsonToken *json_obj_get(JsonToken *object, const char *key) {
    assert(object);
    ASSERT(object->type == JS_TOKEN_OBJECT, "JSON token must be of object type.");

    Hashtable *hashtable = (Hashtable *)object->data;

    return hashtable_get(hashtable, key);
}

int json_obj_get_int(JsonToken *object, const char *key) {
    JsonToken *value = json_obj_get(object, key);

    if (value == NULL) {
        return 0;
    }

    ASSERT(value->type == JS_TOKEN_INTEGER, "Value is not of integer type.");

    return *((i64 *)value->data);
}

f64 json_obj_get_f64(JsonToken *object, const char *key) {
    JsonToken *value = json_obj_get(object, key);
    
    if (value == NULL) {
        return 0.0;
    }

    ASSERT(value->type == JS_TOKEN_NUMBER, "Value is not of number type.");

    return *((f64 *)value->data);
}

bool json_obj_get_bool(JsonToken *object, const char *key) {
    JsonToken *value = json_obj_get(object, key);
    
    if (value == NULL) {
        return false;
    }

    ASSERT(value->type == JS_TOKEN_BOOLEAN, "Value is not of boolean type.");

    return *((bool *)value->data);
}

String *json_obj_get_string(JsonToken *object, const char *key) {
    JsonToken *value = json_obj_get(object, key);
    
    if (value == NULL) {
        return NULL;
    }

    ASSERT(value->type == JS_TOKEN_STRING, "Value is not of string type.");

    return (String *)value->data;
}

JsonToken *json_obj_get_array(JsonToken *object, const char *key) {
    JsonToken *value = json_obj_get(object, key);

    if (value == NULL) {
        return NULL;
    }

    ASSERT(value->type == JS_TOKEN_ARRAY, "Value is not of array type.");

    return value;
}

JsonToken *json_obj_get_object(JsonToken *object, const char *key) {
    JsonToken *value = json_obj_get(object, key);

    if (value == NULL) {
        return NULL;
    }

    ASSERT(value->type == JS_TOKEN_OBJECT, "Value is not of object type.");

    return value;
}

JsonToken *json_arr_get(JsonToken *array, u32 index) {
    assert(array);
    ASSERT(array->type == JS_TOKEN_ARRAY, "JSON token must be of array type.");

    Vector *vector = (Vector *)array;
    ASSERT(index < vector->size, "Index out of bounds.");

    return vector_index(vector, index);
}

int json_arr_get_int(JsonToken *array, u32 index) {
    JsonToken *value = json_arr_get(array, index);
    ASSERT(value->type == JS_TOKEN_INTEGER, "Value is not of integer type.");

    return *((i64 *)value->data);
}

f64 json_arr_get_f64(JsonToken *array, u32 index) {
    JsonToken *value = json_arr_get(array, index);
    ASSERT(value->type == JS_TOKEN_NUMBER, "Value is not of number type.");

    return *((f64 *)value->data);
}

bool json_arr_get_bool(JsonToken *array, u32 index) {
    JsonToken *value = json_arr_get(array, index);
    ASSERT(value->type == JS_TOKEN_BOOLEAN, "Value is not of boolean type.");

    return *((bool *)value->data);
}

String *json_arr_get_string(JsonToken *array, u32 index) {
    JsonToken *value = json_arr_get(array, index);
    ASSERT(value->type == JS_TOKEN_STRING, "Value is not of string type.");

    return (String *)value->data;
}

JsonToken *json_arr_get_array(JsonToken *array, u32 index) {
    JsonToken *value = json_arr_get(array, index);
    ASSERT(value->type == JS_TOKEN_ARRAY, "Value is not of array type.");

    return value;
}

JsonToken *json_arr_get_object(JsonToken *array, u32 index) {
    JsonToken *value = json_arr_get(array, index);
    ASSERT(value->type == JS_TOKEN_OBJECT, "Value is not of object type.");

    return value;
}

JsonToken *json_build_from_tokens(jsmntok_t *tokens, const char *js) {
    size_t count = 0;
    while (true) {
        jsmntok_t token = tokens[count++];
        if (token.start == 0 && token.end == 0) {
            break;
        }
    }

    return json_build_tokens_length(tokens, count - 1, js);
}

JsonToken *json_build_tokens_length(jsmntok_t *tokens, size_t num, const char *js) {
    JsonToken *jsonTokens = calloc(num, sizeof(JsonToken));

    int currentId = 0;
    for (u32 i = 0; i < num; ++i) {
        json_token_create(&jsonTokens[i], tokens[i], currentId++, js);
    }

    for (u32 i = 0; i < num - 1; ++i) {
        JsonToken *token = &jsonTokens[i];
        if (token->type == JS_TOKEN_OBJECT) {
            bool isKey = true;
            String *key = NULL;
            for (u32 j = i + 1; j < num; ++j) {
                JsonToken *next = &jsonTokens[j];
                if (next->parent == token->id) {
                    if (isKey) {
                        key = (String *)next->data;
                        isKey = false;
                    } else {
                        Hashtable *table = (Hashtable *)token->data;
                        hashtable_insert(table, key->characters, next);
                        isKey = true;
                    }
                }
            }
        } else if (token->type == JS_TOKEN_ARRAY) {
            for (u32 j = i + 1; j < num; ++j) {
                JsonToken *next = &jsonTokens[j];
                if (next->parent == token->id) {
                    Vector *array = (Vector *)token->data;
                    vector_add(array, next);
                }
            }
        }
    }

    return jsonTokens;
}