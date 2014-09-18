#include "json.h"

void json_load_file(const char *filename, char *buffer) {
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

    buffer = calloc(fileSize, sizeof(char));
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
}

jsmntok_t *json_tokenize(char *js) {
    
}