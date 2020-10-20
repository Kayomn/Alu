#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
	size_t size;

	size_t length;

	char const * pointer;
} String;

String string_from(char const * restrict pointer) {
	String string = {
		.size = 0,
		.length = 0,
		.pointer = pointer
	};

	for (size_t i = 0; pointer[i] != 0; i += 1) {
		string.length += ((pointer[i] & 0xC0) != 0x80);
		string.size += 1;
	}

	return string;
}

typedef enum {
	Token_Symbol
} Token;

typedef struct {
	Token token;

	String value;
} Lexeme;

int main() {
	String source = string_from("local value = 0");
	Lexeme lexeme;
	size_t cursor;

	while (cursor < source.size) {
		cursor += 1;
	}

	printf("%s\n", source.pointer);
}
