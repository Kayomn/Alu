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

static bool lexeme_matches(Lexeme const * lexeme, char const * zstring) {
	size_t identifierRange;
	size_t lexemeLength = lexeme->end;

	while (*(zstring + identifierRange)) identifierRange += 1;

	if (lexemeLength != identifierRange) return false;

	for (size_t i = 0; i < lexemeLength; i += 1) if (lexeme->start[i] != zstring[i]) return false;

	return true;
}

static char const * keywords[] = {"var"};

enum { KeywordCount = (sizeof(keywords) / sizeof(char const *))};

static bool lexeme_isKeyword(Lexeme const * lexeme, size_t const textRange) {
	for (size_t i = 0; i < KeywordCount; i += 1) {
		if (lexeme_matches(lexeme, keywords[i])) return true;
	}

	return false;
}

static inline bool lexeme_isSymbol(Lexeme const * lexeme, char const symbolCharacter) {
	return ((lexeme->token == Token_Symbol) && (lexeme->start[0] == symbolCharacter));
}

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
				.end = end,
				.start = (source + lexer->cursor)
			};

			lexeme->token = (
				lexeme_isKeyword(lexeme, end) ?
				Token_Keyword :
				Token_Identifier
			);

			lexer->cursor += end;

			return true;
		}
	}

	return false;
}

static inline ASTExpression astExpression_error(char const * message) {
	return (ASTExpression){.as = {.error = message}, .type = ASTExpressionType_Error};
}

static inline ASTStatement astStatement_error(char const * message) {
	return (ASTStatement){.as = {.error = message}, .type = ASTStatementType_Error};
}

static inline ASTStatement astStatement_var(
	Lexeme const * typenameLexeme,
	Lexeme const * identifierLexeme,
	ASTExpression expression
) {
	ASTStatementVar * var = make(ASTStatementVar);
	var->typenameLexeme = (*typenameLexeme);
	var->identifierLexeme = (*identifierLexeme);
	var->expression = expression;

	return (ASTStatement){.as = {.var = var}, .type = ASTStatementType_Var,};
}

static ASTExpression parseExpression(Lexer * lexer, Lexeme * const symbol) {
	return astExpression_error("Not implemented");
}

static ASTStatement parseDeclaration(Lexer * lexer, Lexeme * const keyword) {
	if (keyword->token == Token_Keyword) {
		if (lexeme_matches(keyword, "var")) {
			Lexeme identifier;

			if (lexer_next(lexer, &identifier) && (keyword->token == Token_Identifier)) {
				Lexeme symbol;

				if (lexer_next(lexer, &symbol) && lexeme_isSymbol(&symbol, '=')) {
					if (lexer_next(lexer, &symbol)) {
						return astStatement_var(
							keyword,
							&identifier,
							parseExpression(lexer, &symbol)
						);
					}

					return astStatement_error("Unexpected end of line at `=`");
				}

				return astStatement_error("Unexpected variable declaration without expression");
			}

			return astStatement_error("Expected identifier after `var`");
		} else {
			// TODO: "while", "if", "func", "proc", etc...
		}
	}

	return astStatement_error("Expected keyword at beginning of declaration");
}

static ASTStatement * astStatementList_append(ASTStatementList * list, ASTStatement statement) {
	// TODO: Implement.
	return NULL;
}

ASTStatementList parse(String const source) {
	Lexer lexer = {0, source};
	ASTStatementList statements = {};
	Lexeme declaration;

	while (lexer_next(&lexer, &declaration)) {
		if (!astStatementList_append(&statements, parseDeclaration(&lexer, &declaration))) {
			// Unrecoverable out of memory error.
		}
	}

	return statements;
}
