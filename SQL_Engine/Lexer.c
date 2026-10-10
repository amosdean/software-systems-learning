#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <ctype.h>
#include "lexer.h"

// Source
char *source;
int source_length;

// current token navigation 
int cur_start;
int cur_index;
int cur_line;

// Tokens
// int current_index;
struct Token *tokens;
int token_index = 0;
int state;

void addToken(enum TokenType type) {
    struct Token token;
    char *str; 

    memcpy(str, source + cur_start, cur_index - cur_start);
    str[cur_index] = '\0';

    printf("new string added: %s\n", str);

    token.lexeme = str;
    token.line = cur_line;
    token.type = type; // need to map - state != type currently 

    // add token to tokens
    tokens[token_index] = token;
    token_index++;
}

char advance() {
    cur_index++;
    if(source[cur_index] == '\n') {
        cur_index++;
        cur_line++;
        if(cur_index == source_length) {
            return '\0';
        }
    };
    return source[cur_index];
}

void scanToken() {
    
    char c = source[cur_start];
    int eot = 0;
    while(!eot) {
        switch (state) {
            case START:
                if(isdigit(c)) {
                    state = IN_NUMBER;
                    printf("number: %c\n", c);
                }
                else if(isalpha(c)) {
                    state = IN_IDENTIFIER;
                    printf("letter: %c\n", c);
                }
                break;
            case IN_IDENTIFIER:
                if(isspace(c)) {
                    eot = 1;
                }
                else if(!isalpha(c)) {
                    printf("ERROR: not letter: %c\n", c);
                }
                break;
            case IN_NUMBER:
                if(isspace(c)) {
                    eot = 1;
                }
                else if(isalpha(c)) {
                    printf("ERROR: not number: %c\n", c);
                }
                break;
        }
        c = advance();
        if(c == '\0') {
            eot = 1;
        }
    }
    addToken(state);
    state = START;
    // switch (c) {
    //     case '(': addToken(LEFT_PAREN); break;
    //     case ')': addToken(RIGHT_PAREN); break;
    //     case '{': addToken(LEFT_BRACE); break;
    //     case '}': addToken(RIGHT_BRACE); break;
    //     case ',': addToken(COMMA); break;
    //     case '.': addToken(DOT); break;
    //     case '-': addToken(MINUS); break;
    //     case '+': addToken(PLUS); break;
    //     case ';': addToken(SEMICOLON); break;
    //     case '*': addToken(STAR); break; 
    // }
}

struct Token * scanTokens(char *src, size_t size) {
    source = src;
    state = START;

    tokens = calloc(100, sizeof(struct Token));
    cur_start = 0;
    cur_index = 0;
    cur_line = 1;
    while (cur_index < (int) size) {
      cur_start = cur_index;
      scanToken();
    }

    // tokens.add(new Token(EOF, "", null, line));
    return tokens;
}