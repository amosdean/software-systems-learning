#ifndef lexer_H
#define lexer_H

typedef enum {
    TOK_SELECT,
    TOK_FROM,
    TOK_IDENTIFIER,
    TOK_NUMBER,
    TOK_SEMICOLON,
    TOK_EOF
} TokenType;

static const char *keywords[] = {
    "SELECT",
    "FROM",
    "WHERE"
    "INSERT",
    "INTO",
};

static const int NUMBER_OF_KEYWORDS = 5;

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