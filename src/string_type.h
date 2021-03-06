#ifndef _STRING_TYPE_H_
#define _STRING_TYPE_H_

#define S(x) string_create(x)

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

typedef struct vector_t Vector;

typedef struct string_t {
	char *characters;
	size_t length;
	size_t capacity;
} String;

String *string_new();
void string_free(String *self);
void string_free_void(void *pself);
String *string_reserve(size_t capacity);
String *string_create(const char *cstr);
String *string_clone(const String *other);
void string_resize(String *self, size_t capacity);
void string_copy(const String *src, String *dest);
void string_zero(String *self);
void string_assert(const String *self);

void string_append(String *self, const char *cstr);
void string_erase_at(String *self, size_t index);
void string_erase_from(String *self, size_t index);
void string_erase(String *self, size_t start, size_t end);

void string_print(String *self);

void string_trim(String *self);
int string_char_at(String *self, char find);
void string_lower(String *self);
void string_upper(String *self);
String *string_substring(String *self, size_t start, size_t end);
Vector *string_split(String *self, char delimeter);
Vector *string_splitws(String *self);

bool _char_is_whitespace(char c);

#endif