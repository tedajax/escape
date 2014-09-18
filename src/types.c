#include "types.h"

void internal_print_assert(char *file,
                           int line,
                           const char *msg1,
                           const char *msg2,
                           const char *msg3) {
    if (!msg1) {
        fprintf(stderr, "--------------\nASSERT\n");
    } else if (!msg2) {
        fprintf(stderr, "--------------\nASSERT -- %s\n", msg1);
    } else if (!msg3) {
        fprintf(stderr, "--------------\nASSERT -- %s%s\n", msg1, msg2);
    } else {
        fprintf(stderr, "--------------\nASSERT -- %s%s%s\n", msg1, msg2, msg3);
    }
    fprintf(stderr, "\nFile: %s -- Line: %d\n--------------\n", file, line);
}