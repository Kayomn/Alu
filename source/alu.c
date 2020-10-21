#include "alu.h"

static inline bool isWhitespace(char const c) {
	switch (c) {
		case '\n':
		case '\r':
		case '\v':
		case '\f':
		case '\t':
		case ' ':  return true;

		default: return false;
	}
}

static inline bool isSymbol(char const c) {
	switch (c) {
		case '+':
		case '-':
		case '!':
		case '=':
		case '<':
		case '>': return true;

		default: return false;
	}
}

static bool lexer_matches(char const * text, size_t const textRange, char const * zstring) {
	size_t identifierRange;

	while (*(zstring + identifierRange)) identifierRange += 1;

	if (textRange != identifierRange) return false;

	for (size_t i = 0; i < textRange; i += 1) if (text[i] != zstring[i]) return false;

	return true;
}

static char const * keywords[] = {"var"};

enum { KeywordCount = (sizeof(keywords) / sizeof(char const *))};

static bool lexer_isKeyword(char const * text, size_t const textRange) {
	for (size_t i = 0; i < KeywordCount; i += 1) {
		if (lexer_matches(text, textRange, keywords[i])) return true;
	}

	return false;
}

typedef enum {
	Token_Symbol,
	Token_Keyword,
	Token_Identifier,
} Token;

typedef struct {
	Token token;

	size_t end;

	char const * start;
} Lexeme;

typedef struct {
	size_t cursor;

	String source;
} Lexer;

static inline bool lexer_endOfFile(Lexer * lexer) {
	return (lexer->cursor == lexer->source.size);
}

static bool lexer_next(Lexer * lexer, Lexeme * lexeme) {
	char const * source = string_chars(&lexer->source);

	while (!lexer_endOfFile(lexer)) {
		char const c = source[lexer->cursor];

		if (isWhitespace(c)) {
			// Ignore whitespace.
			lexer->cursor += 1;
		} else if (isSymbol(c)) {
			// Single or multi-character symbol.
			size_t end = 1;

			if ((!lexer_endOfFile(lexer)) && (source[lexer->cursor + end] == '=') && isSymbol(c)) {
				end += 1;
			}

			(*lexeme) = (Lexeme){
				.token = Token_Symbol,
				.end = end,
				.start = (source + lexer->cursor)
			};

			lexer->cursor += end;

			return true;
		} else {
			size_t end = 1;

			while ((!lexer_endOfFile(lexer)) && (!isWhitespace(c)) && (!isSymbol(c))) end += 1;

			(*lexeme) = (Lexeme){
				.token = (
					lexer_isKeyword((source + lexer->cursor), end) ?
					Token_Keyword :
					Token_Identifier
				),
				.end = end,
				.start = (source + lexer->cursor)
			};

			lexer->cursor += end;

			return true;
		}
	}

	return false;
}

typedef struct {} ASTNode;

typedef struct {
	size_t count;

	ASTNode * nodes;
} AST;

static char const * parseKeyword(Lexer * const lexer, Lexeme * const keyword) {
	if (keyword->token != Token_Keyword) return "Expected keyword";

	if (lexeme_matches(keyword, "var")) {
		Lexeme identifier;

		if ((!lexer_next(&lexer, &identifier)) || (keyword->token != Token_Identifier)) {
			return "Expected identifier after `local`";
		}

		Lexeme symbol;

		if (lexer_next(&lexer, &identifier) && lexeme_isSymbol(&symbol, '=')) {
			// TODO: Parse expression.
		}
	}

	return NULL;
}

char const * parse(String const source) {
	Lexer lexer = {0, source};
	Lexeme declaration;

	while (lexer_next(&lexer, &declaration)) {
		char const * error = parseKeyword(&lexer, &declaration);

		if (error) return error;
	}

	return NULL;
}
