#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"


const Action LEXER_TRANSITIONS[4][4] = {
        //          LETTER    NUMBER     WHITESPACE     SEMICOLON
/* START */     {TO_WORD,    TO_NUMBER,   CONT,         ADD_TOKEN},
/* KEYWORD */   {CONT,       ERR,         ADD_TOKEN,    ADD_TOKEN},
/* NUMBER */    {ERR,        CONT,        ADD_TOKEN,    ADD_TOKEN},
/* PUNCT */     {ADD_TOKEN,  ADD_TOKEN,   ADD_TOKEN,    ADD_TOKEN}
};

// Source
static char *source;
static int source_length;

// current token navigation 
static int cur_start;
static int cur_index;
static int cur_line;

// Tokens
static Token **tokens;
static const int NUM_OF_TOKENS_AVAILABLE = 100;
static int token_index;
static int state;

CType getTokenType(char c) {
    if(isalpha(c)) {
        return LETTER;
    }
    else if(isdigit(c)) {
        return NUMBER;
    }
    else if(ispunct(c)) {
        return PUNCTUATION;
    }
    else if(isspace(c)) {
        return WHITESPACE;
    }
    else {
        return EOF;
    }
}

TokenType getPunctuationTokenType(char c) {
    switch(c) {
        case ';':
            return TOK_SEMICOLON;
    }
}

void addToken(TokenType type) {
    if (token_index >= NUM_OF_TOKENS_AVAILABLE) {
        fprintf(stderr, "Token Limit Exceeded\n");
        return;
    }
    Token *token = calloc(1, sizeof *token);

    token->type = type;
    token->line = cur_line;
    char *str;
    if(type != TOK_EOF){
        // Important for self terminating tokens like punctuation
        int cur_end = (cur_index != cur_start) ? cur_index : cur_index + 1; 
        size_t lexeme_len = (size_t)(cur_end - cur_start);
        const char *text;
        text = source + cur_start;
        lexeme_len = (size_t)(cur_end - cur_start);
        str = malloc(lexeme_len + 1);

        memcpy(str, source + cur_start, lexeme_len);
        str[lexeme_len] = '\0';

        token->lexeme = str;
        printf("string added: \"%s\"\n", str); // Debugging
    }
    // Keyword check
    if(type == TOK_IDENTIFIER) { 
        for(int i = 0; i < NUMBER_OF_KEYWORDS; i++) {
            if(!strcmp(strupr(str), keywords[i])) {
                token->type = i; 
                break;
            }
        }
    }
    printf("type: %d\n", token->type); // Debugging

    // add token to tokens
    tokens[token_index++] = token;
}

char advance() {
    cur_index++;
    if(source[cur_index] == '\n') {
        cur_index++;
        cur_line++;
        if(cur_index == source_length)
            return '\0';
    };
    return source[cur_index];
}

void scanToken() {
    char c = source[cur_start];
    int eot = 0;
    Action action;
    CType ctype;

    while(!eot) {
        ctype = getTokenType(c);
        action = LEXER_TRANSITIONS[state][ctype];

        switch (action) {
            case TO_WORD:
                state = LEX_IN_IDENTIFIER;
                break;
            case TO_NUMBER:
                state = LEX_IN_NUMBER;
                break;
            case CONT:
                if(state == LEX_START) 
                    cur_start++;
                break;
            case ADD_TOKEN:
                if (state == LEX_IN_IDENTIFIER) 
                    addToken(TOK_IDENTIFIER);
                else if (state == LEX_IN_NUMBER) 
                    addToken(TOK_NUMBER);
                else if (ctype == PUNCTUATION) 
                    addToken(getPunctuationTokenType(c));
                eot = 1;
                break;
            case ERR:
                printf("ERROR at line %d, index %d, for character %c\n",cur_line, cur_index, c);
                eot = 1;
                break;
        }
        if (action == ADD_TOKEN &&
            ctype == PUNCTUATION &&
            state != LEX_START) 
            break;

        c = advance();
        // Check for end of text before continuing the loop to avoid reading beyond the buffer
        if (c == '\0') eot = 1;
    }
}

Token **scanTokens(char *src, size_t size, int *num_of_tokens) {
    source = src;

    tokens = calloc(NUM_OF_TOKENS_AVAILABLE, sizeof(tokens));
    cur_start = 0;
    cur_index = 0;
    cur_line = 1;
    token_index = 0;

    while (cur_index < (int) size) {
      cur_start = cur_index;
      state = LEX_START;
      scanToken();
    }

    addToken(TOK_EOF);
    *num_of_tokens = token_index;
    return tokens;
}