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
             valid |= c == '\t';
        
        if (!valid) {
            return false;
        }
    }

    return true;
}

Vector *parse_words(String *input) {
    //TODO: split by whitespace instead of just single spaces
    Vector *words = string_splitws(input);
    return words;
}

Verb parse_verb(Vector *words) {
    assert(words);
    assert(words->size > 0);

    String *verbWord = (String *)vector_index(words, 0);
    Verb *ptrVerb = (Verb *)hashtable_get(G_WORD_TABLE_VERBS, verbWord->characters);
    
    Verb result = VERB_INVALID;
    if (ptrVerb) {
        result = *ptrVerb;
    }

    return result;
}

Action parse_action(Vector *words) {
    assert(words);
    assert(words->size > 0);

    Action result;
    result.verb = parse_verb(words);
    result.subjId = -1;
    result.secSubjId = -1;

    return result;
}