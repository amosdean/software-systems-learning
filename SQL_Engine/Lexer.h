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

static const int number_of_keywords = 5;


enum lexerState {
    START
    ,IN_IDENTIFIER
    ,IN_NUMBER
};

typedef struct Token {
	TokenType type;
	char * lexeme;
    // Keyword keyword;
    int line;
} Token;
Token ** scanTokens(char *src, size_t size);

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