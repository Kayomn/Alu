#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define make(type) calloc(1, sizeof(type))

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

char const * string_chars(String * const str) {
	return ((str->length > String_StaticBufferSize) ? str->raw.dynamic : str->raw.static_);
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

typedef enum {
	ASTExpressionType_Error,
} ASTExpressionType;

typedef struct {
	union {
		char const * error;
	} as;

	ASTExpressionType type;
} ASTExpression;

typedef struct {
	Lexeme typenameLexeme;

	Lexeme identifierLexeme;

	ASTExpression expression;
} ASTStatementVar;

typedef enum {
	ASTStatementType_Error,
	ASTStatementType_Var
} ASTStatementType;

typedef struct {
	union {
		char const * error;

		ASTStatementVar * var;
	} as;

	ASTStatementType type;
} ASTStatement;

typedef struct {
	size_t count;

	ASTStatement * pointer;
} ASTStatementList;

ASTStatementList parse(String const source);

