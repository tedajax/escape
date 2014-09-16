#include <stdio.h>
#include "types.h"
#include "vector.h"

int main(int argc, char *argv[]) {
	String *test = S("Testing123");
    string_print(test);
    string_free(test);

    String *subTest = S("123456789");
    String *sub = string_substring(subTest, 1, 3);
    string_print(sub);
    string_free(sub);
    string_free(subTest);

    String *splitTest = S("abc 123 def 456");
    Vector *tokens = string_split(splitTest, ' ');

    char *splitcstr = string_cstr(splitTest);
    printf("Splitting %s :\n", splitcstr);
    free(splitcstr);
    for (u32 i = 0; i < tokens->size; ++i) {
        String *s = (String *)vector_index(tokens, i);
        string_print(s);
    }

    string_free(splitTest);
    vector_free(tokens);

    String *lowerTest = S("WHAT about LOVE!!?!?!?!?11234");
    string_tolower(lowerTest);
    string_print(lowerTest);
    string_free(lowerTest);

    String *upperTest = S("nioasiq QWEVaklfq f!3322");
    string_toupper(upperTest);
    string_print(upperTest);
    string_free(upperTest);

    String *blah = S("blah");
    string_append(blah, " blah blah blah blah");
    string_print(blah);
    string_free(blah);

    return 0;
}