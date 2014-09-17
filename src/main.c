#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "types.h"
#include "containers.h"
#include "tables.h"
#include "parser.h"

const size_t MAX_INPUT_LENGTH = 64;

int main(int argc, char *argv[]) {
    char *input = calloc(MAX_INPUT_LENGTH, sizeof(char));
    bool run = init_tables();

    while (run) {
        //zero out input
        memset(input, 0, MAX_INPUT_LENGTH);

        printf("> ");
        //scanf("%63s", input);
        fgets(input, MAX_INPUT_LENGTH, stdin);

        //strip new line character
        size_t inputLen = strlen(input);
        input[inputLen - 1] = '\0';

        String *inputStr = S(input);

        if (!parse_input_valid(inputStr)) {
            printf("I don't understand...\n");
            string_free(inputStr);
            continue;
        }

        Vector *words = parse_words(inputStr);
        Verb verb = parse_verb(words);

        vector_free(words);
        string_free(inputStr);

        printf("Verb: %d\n", verb);

        if (verb == VERB_QUIT) {
            run = false;
        }
    }

    free(input);

    printf("Done...\n");

    return 0;
}