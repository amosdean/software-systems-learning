#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"


const Action TRANSITIONS[3][3] = {
        //          LETTER    NUMBER     WHITESPACE 
/* START */     {TO_WORD,    TO_NUMBER,   CONT},
/* KEYWORD */   {CONT,       ERR,         ADD_TOKEN},
/* NUMBER */    {ERR,        CONT,        ADD_TOKEN}
};

// Source
char *source;
int source_length;

// current token navigation 
int cur_start;
int cur_index;
int cur_line;

// Tokens
// int current_index;
Token *tokens[];
int token_index = 0;
int state;

void addToken(enum TokenType type) {
    Token *token = calloc(1, sizeof(Token));
    char *str = calloc(100, sizeof(char)); 

    memcpy(str, source + cur_start, cur_index - cur_start);
    str[cur_index] = '\0';


    token->lexeme = str;
    token->line = cur_line;
    switch(state) {
        case IN_IDENTIFIER:
            for(int i = 0; i < number_of_keywords; i++) {
                if(!strcmp(strupr(str), keywords[i])) {
                    token->type = KEYWORD;
                    break;
                }
                if(i + 1 == number_of_keywords)
                    token->type = IDENTIFIER;
            }
            break;
        case IN_NUMBER:
            token->type = LITERAL;
            break;
    }
    // token.type = type; // need to map - state != type currently 
    printf("string added: %s\n", str);
    printf("type: %d\n", token->type);

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
    Action action;
    CType ctype;
    while(!eot) {
        if(isdigit(c)){
            ctype = NUMBER;
        }
        else if(isalpha(c)) {
            ctype = LETTER;
        }
        // else if(c == '*')

        // else if(isspace(c)) {
        else {
            ctype = WHITESPACE;
        }
        action = TRANSITIONS[state][ctype];
        switch (action) {
            case TO_WORD:
                state = IN_IDENTIFIER;
                break;
            case TO_NUMBER:
                state = IN_NUMBER;
                break;
            case CONT:
                if(state == START) 
                    cur_start++;
                break;
            case ADD_TOKEN:
                addToken(state);
                eot = 1;
                break;
            case ERR:
                printf("ERROR at line %d, index %d, for character %c\n",cur_line, cur_index, c);
                eot = 1;
                break;
        }
        c = advance();
        if(c == '\0') {
            eot = 1;
            addToken(END_OF_FILE);
        }
    }
    // addToken(state);
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

Token ** scanTokens(char *src, size_t size) {
    source = src;

    *tokens = calloc(100, sizeof(struct Token));
    cur_start = 0;
    cur_index = 0;
    cur_line = 1;
    while (cur_index < (int) size) {
      cur_start = cur_index;
      state = START;
      scanToken();
    }

    // tokens.add(new Token(EOF, "", null, line));
    return tokens;
}