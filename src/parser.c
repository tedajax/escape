#include "parser.h"

bool parse_input_valid(String *input) {
    if (input->length == 0) {
        return false;
    }

    //First lowercase everything
    string_lower(input);

    for (size_t i = 0; i < input->length; ++i) {
        char c = input->characters[i];
        bool valid = c == ' ';
             valid |= (c >= 'a' && c <= 'z');
             valid |= (c >= '0' && c <= '9');
        
        if (!valid) {
            return false;
        }
    }

    return true;
}

Vector *parse_words(String *input) {
    //TODO: split by whitespace instead of just single spaces
    Vector *words = string_split(input, ' ');
    return words;
}

Verb parse_verb(Vector *words) {
    assert(words);
    assert(words->size > 0);

    String *verbWord = (String *)vector_index(words, 0);
    char *verbWordCstr = string_cstr(verbWord);
    Verb *ptrVerb = (Verb *)hashtable_get(G_WORD_TABLE_VERBS, verbWordCstr);
    
    free(verbWordCstr);

    Verb result = VERB_INVALID;
    if (ptrVerb) {
        result = *ptrVerb;
    }

    return result;
}