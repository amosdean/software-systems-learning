#ifndef parser_H
#define parser_H
#include "lexer.h"

typedef enum ParserState {
     PS_START
    ,PS_TO_SELECT
    ,PS_SELECT_COL
    ,PS_TO_FROM
    ,PS_FROM_TABLE
    ,PS_TO_WHERE
    ,PS_W_COND
} ParserState;

typedef enum ParserAction {
    PA_TO_SELECT
    ,PA_SEL_COL
    ,PA_TO_FROM
    ,PA_TO_WHERE
    ,PA_CONT
    ,PA_TO_START
    ,PA_FROM_TBL
    ,PA_ERR
    ,PA_END
} ParserAction;

typedef enum StatementType {
    ST_SELECT
    ,ST_CREATE
    ,ST_INSERT
} StatementType;

typedef struct TreeNode {
    Token *token;
    struct TreeNode *firstChild;
    struct TreeNode *nextSiblingNode;
} TreeNode;

typedef struct Statement {
    TreeNode *rootNode;
    StatementType type;
    struct Statement *next;
} Statement;

typedef struct Parser {
    // ParserState state;
    int eot; // end of tokens
    int eos; // end of statements
    Token **tokens;
    Token *current_token;
    Statement *rootStatement;
    Statement *currentStatement;
    TreeNode *currentNode;
    int tokens_size;
    int tokens_index;
} Parser;

int parse(Token **src_tokens, int *tokensSize); 

#endif // parser_H