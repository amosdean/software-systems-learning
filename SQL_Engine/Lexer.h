#ifndef lexer_H
#define lexer_H

enum TokenType {
    KEYWORD, IDENTIFIER, PUNCUATION, OPERATOR, CONSTANT, LITERAL,
    // Keywords
    AND, OR, CASE, WHEN, END, ELSE, FALSE,
    PRINT, TRUE,

    // Single-character tokens
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, STAR,

    // One or two character tokens
    EQUAL, BANG_EQUAL, 
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,
    NOT_EQUAL,

    // Literals
    // IDENTIFIER, STRING, I,

    END_OF_FILE
};

static const int number_of_keywords = 5;
static const char *keywords[] = {
    "CREATE",
    "TABLE",
    "SELECT",
    "FROM",
    "WHERE"
};
// typedef enum Keyword {
    // CREATE,
    // TABLE,
    // SELECT,
    // FROM,
    // WHERE
// } Keyword;

enum lexerState {
    START
    ,IN_IDENTIFIER
    ,IN_NUMBER
};

typedef struct Token {
	enum TokenType type;
	char * lexeme;
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
    WHITESPACE
} CType;


#endif // lexer_H