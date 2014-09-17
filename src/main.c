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

    String *charAtTest = S("12345");
    printf("%d\n", string_char_at(charAtTest, '2'));
    string_free(charAtTest);

    String *lowerTest = S("WHAT about LOVE!!?!?!?!?11234");
    string_lower(lowerTest);
    string_print(lowerTest);
    string_free(lowerTest);

    String *upperTest = S("nioasiq QWEVaklfq f!3322");
    string_upper(upperTest);
    string_print(upperTest);
    string_free(upperTest);

    String *blah = S("blah");
    string_append(blah, " blah blah blah blah");
    string_print(blah);
    string_free(blah);

    String *eraseTest = S("erase me");
    string_erase(eraseTest, 1, 3);
    string_print(eraseTest);
    string_free(eraseTest);

    String *trimTest = S("   test   ");
    string_trim(trimTest);
    string_print(trimTest);
    string_free(trimTest);

    String *eraseFromTest = S("123456789");
    string_erase_from(eraseFromTest, 3);
    string_print(eraseFromTest);
    string_free(eraseFromTest);

    String *eraseAtTest = S("12345");
    string_erase_at(eraseAtTest, 2);
    string_print(eraseAtTest);
    string_free(eraseAtTest);

    return 0;
}