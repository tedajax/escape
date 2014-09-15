#include "string_type.h"

String *string_new() {
	return string_reserve(16);
}

void string_free(String *self) {
	string_assert(self);
	free(self->characters);
	free(self);
}

String *string_reserve(size_t capacity) {
	String *self = calloc(1, sizeof(String *));

	self->length = 0;
	self->capacity = capacity;
	self->characters = calloc(self->capacity, sizeof(char));

	return self;
}

String *string_create(const char *cstr) {
	assert(cstr);

	size_t clen = strlen(cstr);

	String *self = string_reserve(clen);

	for (size_t i = 0; i < clen; ++i) {
		self->characters[i] = cstr[i];
	}

	return self;
}

String *string_clone(const String *other){
	string_assert(other);

	String *self = string_reserve(other->capacity);

	for (size_t i = 0; i < other->length; ++i) {
		self->characters[i] = other->characters[i];
	}

	return self;
}

void string_resize(String *self, size_t capacity) {
	assert(self);
	assert(self->characters);

	size_t oldCapacity = self->capacity;

	self->capacity = capacity;
	if (self->length > self->capacity) {
		self->length = self->capacity;
	}

	char *newChars = calloc(self->capacity, sizeof(char));

	if (oldCapacity > self->capacity) {
		oldCapacity = self->capacity;
	}

	for (size_t i = 0; i < oldCapacity; ++i) {
		newChars[i] = self->characters[i];
	}

	free(self->characters);

	self->characters = newChars;
}

void string_copy(const String *src, String *dest) {
	assert(src);
	assert(dest);

	string_zero(dest);

	string_resize(dest, src->capacity);

	dest->length = src->length;

	for (size_t i = 0; i < src->length; ++i) {
		dest->characters[i] = src->characters[i];
	}
}

void string_zero(String *self) {
	assert(self);

	if (self->characters) {
		free(self->characters);
	}

	self->characters = NULL;
	self->capacity = 0;
	self->length = 0;
}

void string_assert(const String *self) {
	assert(self);
	assert(self->characters);
}

void string_append(String *self, const char *cstr) {
	string_assert(self);
	assert(cstr);

	size_t clen = strlen(cstr);
	size_t totalLength = self->length + clen;

	if (totalLength > self->capacity) {
		size_t newCapacity = totalLength;
		if (self->capacity * 2 > newCapacity) {
			newCapacity = self->capacity * 2;
		}
		string_resize(self, newCapacity);
	}

	for (size_t i = 0; i < clen; ++i) {
		self->characters[self->length + i] = cstr[i];
	}

	self->length = totalLength;
}

void string_removeAt(String *self, size_t index) {
	string_assert(self);
	assert(index < self->length);

	for (size_t i = index + 1; i < self->length; ++i) {
		self->characters[i - 1] = self->characters[i];
	}

	--self->length;
}

void string_erase(String *self, size_t start, size_t end) {
	string_assert(self);
	assert(start < self->length);
	assert(end < self->length);

	size_t removeLen = (end - start) + 1;

	for (size_t i = start + removeLen; i < self->length && i < end + removeLen; ++i) {
		self->characters[i - removeLen] = self->characters[i];
	}

	self->length -= removeLen;
}

void string_cStr(String *self, char *dest) {
	string_assert(self);
	assert(dest);

	snprintf(dest, self->length, "%s", self->characters);
}

void string_print(String *self) {
	string_assert(self);

	printf("%s\n", self->characters);
}

void string_strim(String *self) {
	string_assert(self);

	bool whitespace = true;
	size_t cIndex = 0;

	while (whitespace && cIndex < self->length) {
		whitespace = _char_is_whitespace(self->characters[cIndex]);
		if (whitespace) { ++cIndex; }
	}

	if (cIndex > 0) {
		string_erase(self, 0, cIndex - 1);
	}

	cIndex = self->length - 1;
	whitespace = true;

	while (whitespace && cIndex >= 0) {
		whitespace = _char_is_whitespace(self->characters[cIndex]);
		if (whitespace) { --cIndex; }
	}

	if (cIndex < self->length - 1) {
		string_erase(self, cIndex, self->length - 1);
	}
}

int64_t string_charAt(String *self, char find) {
	string_assert(self);

	for (int64_t i = 0; i < self->length; ++i) {
		if (self->characters[i] == find) {
			return i;
		}
	}

	return -1;
}

void string_toLower(String *self) {
	string_assert(self);

	for (size_t i = 0; i < self->length; ++i) {
		char c = self->characters[i];
		if (c >= 'A' && c <= 'Z') {
			self->characters[i] += 0x20;
		}
	}
}

void string_toUpper(String *self) {
	string_assert(self);

	for (size_t i = 0; i < self->length; ++i) {
		char c = self->characters[i];
		if (c >= 'a' && c <= 'z') {
			self->characters[i] -= 0x20;
		}
	}
}

String *string_substring(String *self, size_t start, size_t end) {
	string_assert(self);
	assert(end >= start);
	assert(start >= 0 && start < self->length);
	assert(end >= 0 && end < self->length);

	size_t resultLen = (end - start) + 1;

	String *result = string_reserve(resultLen);

	for (size_t i = start; i <= end; ++i) {
		result->characters[i - start] = self->characters[i];
	}

	return result;
}



bool _char_is_whitespace(char c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\b' || c == '\r';
}