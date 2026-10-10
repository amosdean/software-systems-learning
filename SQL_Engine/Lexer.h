#ifndef lexer_H
#define lexer_H

enum TokenType {
    // KEYWORD, IDENTIFIER, PUNCUATION, OPERATOR, CONSTANT, LITERAL,
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
    IDENTIFIER, STRING, NUMBER,

    END_OF_FILE
};

enum lexerState {
    START
    ,IN_IDENTIFIER
    ,IN_NUMBER
};

struct Token {
	enum TokenType type;
	char * lexeme;
    int line;
};
struct Token * scanTokens(char *src, size_t size);


#endif // lexer_H