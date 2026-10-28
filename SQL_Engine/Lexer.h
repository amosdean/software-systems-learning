#include <stddef.h>
#ifndef lexer_H
#define lexer_H
// Token types and keywords must match order
typedef enum {
    TOK_SELECT,
    TOK_FROM,
    TOK_WHERE,
    TOK_CREATE,
    TOK_TABLE,
    TOK_INSERT,
    TOK_INTO,
    TOK_VALUES,
    TOK_IDENTIFIER,
    TOK_NUMBER,
    TOK_SEMICOLON,
    TOK_DATATYPE,
    TOK_OTHER,
    TOK_EOF
} TokenType;

static const char *KEYWORDS[] = {
    "SELECT",
    "FROM",
    "WHERE",
    "CREATE",
    "TABLE",
    "INSERT",
    "INTO",
    "VALUES"
};

static const char *DATATYPES[] = {
    "INT",
    "VARCHAR",
    "CHAR",
    "FLOAT",
    "DOUBLE"
};

static const int NUMBER_OF_KEYWORDS = 8;
static const int NUMBER_OF_DATATYPES = 5;

enum lexerState {
    LEX_START
    ,LEX_IN_IDENTIFIER
    ,LEX_IN_NUMBER
    ,LEX_IN_PUNCTUATION
};

typedef struct Token {
	TokenType type;
	char * lexeme;
    // Keyword keyword;
    // DataType datatype;
    int line;
} Token;
Token ** scanTokens(char *src, size_t size, int *num_of_tokens);

typedef enum Action {
    TO_NUMBER
    ,TO_WORD
    ,CONT
    ,ADD_TOKEN
    ,ERR
} Action;

typedef enum CType {
    LETTER,
    NUMBER,
    WHITESPACE,
    PUNCTUATION
} CType;


#endif // lexer_H