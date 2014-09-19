#include "json.h"

const u32 JSON_DEFAULT_HASHTABLE_BUCKETS = 8;
const u32 JSON_DEFAULT_VECTOR_SIZE = 8;
const u32 JSON_DEFAULT_STRING_LENGTH = 12;

const char *json_token_type_string(JsonTokenType type) {
    switch (type) {
        default:
        case JS_TOKEN_NULL: return "null";
        case JS_TOKEN_OBJECT: return "object";
        case JS_TOKEN_ARRAY: return "array";
        case JS_TOKEN_STRING: return "string";
        case JS_TOKEN_NUMBER: return "number";
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

    self->type = type;
    self->id = -1;
    self->children = -1;
    self->parent = -1;

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
            self->data = (void *)string_reserve(JSON_DEFAULT_STRING_LENGTH);
            break;

        case JS_TOKEN_NUMBER:
            self->data = calloc(1, sizeof(f64));
            break;

        case JS_TOKEN_BOOLEAN:
            self->data = calloc(1, sizeof(bool));
            break;
    }

    return self;
}

JsonToken *json_token_create(jsmntok_t token, int id, const char *js) {
    JsonToken *new = NULL;

    JsonTokenType type = JS_TOKEN_NULL;

    char c = '\0';

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
                type = JS_TOKEN_NUMBER;
            } else if (c == 't' || c == 'f') {
                type = JS_TOKEN_BOOLEAN;
            }

            break;
        default: break;
    }

    new = json_token_new(type);
    new->id = id;
    new->parent = token.parent;
    new->children = token.size;

    return new;
}

void json_token_free(JsonToken *self) {
    // TODO: this
}

void json_token_free_void(void *self) {
    json_token_free((JsonToken *)self);
}

void json_token_print(JsonToken *self) {
    printf("{ type: %s, id: %d, children: %d, parent: %d }\n",
        json_token_type_string(self->type),
        self->id,
        self->children,
        self->parent);
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
    int currentId = 0;
    for (u32 i = 0; i < num; ++i) {
        JsonToken *token = json_token_create(tokens[i], currentId, js);
        ++currentId;
        json_token_print(token);
        free(token);
    }

    return NULL;
}