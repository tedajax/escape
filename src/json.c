#include "json.h"

char *json_load_file(const char *filename, char *buffer) {
    FILE *fp;
    long fileSize;

    fp = fopen(filename, "rb");
    ASSERT(fp, "Could not open file: ", filename);

    fseek(fp, 0L, SEEK_END);
    fileSize = ftell(fp);
    rewind(fp);

    // make sure we free the buffer if there's something there.
    if (buffer) {
        free(buffer);
    }

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

jsmntok_t *json_tokenize(char *js) {
    ASSERT(js, "NULL json data passed to json_tokenize.");

    jsmn_parser parser;
    jsmn_init(&parser);

    unsigned int n = MAX_JSON_TOKENS;
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