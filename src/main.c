#include <stdio.h>
#include "types.h"

int main(int argc, char *argv[]) {
	String *test = string_create("Testing123");
    string_print(test);
    string_free(test);
    
    return 0;
}