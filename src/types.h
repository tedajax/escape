#ifndef _TYPES_H_
#define _TYPES_H_

#include <stdlib.h>
#include <signal.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>

#define DEBUG

void internal_print_assert(char *file,
                           int line,
                           const char *msg1,
                           const char *msg2,
                           const char *msg3);

#define ASSERT_ALWAYS(exp, msg1, msg2, msg3)                                \
    do {                                                                    \
        static bool _skipAssert = false;                                    \
        if (!_skipAssert && !(exp)) {                                       \
            internal_print_assert(__FILE__, __LINE__, msg1, msg2, msg3);    \
            _skipAssert = true;                                             \
            exit(EXIT_FAILURE);                                             \
        }                                                                   \
    } while (0);

#ifdef DEBUG
    #define ASSERT_1_ARG(exp) ASSERT_ALWAYS(exp, NULL, NULL, NULL)
    #define ASSERT_2_ARG(exp, msg1) ASSERT_ALWAYS(exp, msg1, NULL, NULL)
    #define ASSERT_3_ARG(exp, msg1, msg2) ASSERT_ALWAYS(exp, msg1, msg2, NULL)
    #define ASSERT_4_ARG(exp, msg1, msg2, msg3) ASSERT_ALWAYS(exp, msg1, msg2, msg3)

    #define GET_5TH_ARG(arg1, arg2, arg3, arg4, arg5, ...) arg5
    #define ASSERT_MACRO_CHOOSER(...) \
        GET_5TH_ARG(__VA_ARGS__, ASSERT_4_ARG, ASSERT_3_ARG, \
                    ASSERT_2_ARG, ASSERT_1_ARG, )

    #define ASSERT(exp, ...) ASSERT_MACRO_CHOOSER(exp, __VA_ARGS__)(exp, __VA_ARGS__)
#else
    #define ASSERT(...) exit(EXIT_ASSERT)
#endif

typedef	int8_t		i8;
typedef	int16_t		i16;
typedef	int32_t		i32;
typedef	int64_t		i64;

typedef uint8_t		u8;
typedef	uint16_t	u16;
typedef	uint32_t	u32;
typedef	uint64_t	u64;

typedef	float		f32;
typedef	double		f64;

#include "string_type.h"

#endif