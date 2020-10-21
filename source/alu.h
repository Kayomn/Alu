#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define cast(type) (type)

enum {String_StaticBufferSize = 16};

typedef struct {
	size_t length;

	size_t size;

	union {
		char const * dynamic;

		char static_[String_StaticBufferSize];
	} raw;
} String;

inline bool string_isDynamic(String * const str) {
	return (str->length > String_StaticBufferSize);
}

char const * string_chars(String * const str) {
	return (string_isDynamic(str) ? str->raw.dynamic : str->raw.static_);
}

char const * parse(String const source);

