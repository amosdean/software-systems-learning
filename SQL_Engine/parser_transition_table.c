#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "lexer.h"
const ParserAction PARSER_TRANSITIONS[6][6] = {
    //                          SELECT          FROM       CREATE   TABLE   INSERT  INTO    VALUES     IDENTIFIER   NUMBER     SEMICOLON      EOF
    /* START */             {PA_TO_SELECT,     PA_ERR,     PA_ERR,     PA_ERR,     PA_ERR,     PA_END},
    /* TO_SELECT */         {PA_ERR,           PA_ERR,     PA_SEL_COL, PA_ERR,     PA_ERR,     PA_ERR},
    /* SELECT_COLUMNS */    {PA_ERR,           PA_TO_FROM, PA_CONT,    PA_CONT,    PA_ERR,     PA_ERR},
    /* TO_FROM */           {PA_ERR,           PA_ERR,     PA_FROM_TBL,PA_ERR,     PA_ERR,     PA_ERR}, 
    /* FROM_TABLE */        {PA_ERR,           PA_ERR,     PA_ERR,     PA_ERR,     PA_TO_START,PA_END},
    /* TO_WHERE */          {PA_ERR,           PA_ERR,     PA_CONT,    PA_ERR,     PA_CONT,    PA_ERR}
};


static ParserState state;
static int eot;
static Token **tokens;
static TreeNode *rootNode;
static TreeNode *currentNode;
// TreeNode *nextNode;
static int tokens_size;
static int tokens_index;

TreeNode * createTreeNode(Token *token) {
    TreeNode *node = calloc(1, sizeof(TreeNode));
    node->token = token;
    node->firstChild = NULL;
    node->nextSiblingNode = NULL;
    return node;
}

void insertSibling(TreeNode *node, Token * token) {
    TreeNode *nextNode = createTreeNode(token);
    node->nextSiblingNode = nextNode;
    currentNode = nextNode;
}

void insertChild(TreeNode *node, Token *token) {
    while(node->firstChild != NULL)
        node = node->firstChild;
    TreeNode *nextNode = createTreeNode(token);
    node->firstChild = nextNode;
}
// char advance() {
//     tokens_index++;
//     return tokens[tokens_index];
// }
void parse(Token *token) {
    ParserAction action = PARSER_TRANSITIONS[state][token->type];
    switch(action) {
        case PA_TO_SELECT:
            state = PS_TO_SELECT;
            rootNode = createTreeNode(token);
            currentNode = rootNode;
            break;
        case PA_SEL_COL:
            state = PS_SELECT_COL;
            insertChild(currentNode, token);
            break;
        case PA_TO_FROM:
            state = PS_TO_FROM;
            insertSibling(currentNode, token);
            break;
        case PA_FROM_TBL:
            state = PS_FROM_TABLE;
            insertChild(currentNode, token);
            break;
        case PA_TO_START:
            state = PS_START;
            break;
        case PA_TO_WHERE:
            state = PS_W_COND;
            insertSibling(currentNode, token);
            break;
        case PA_CONT: 
            insertChild(currentNode, token);
            break;
        case PA_ERR: 
            printf("ERROR: Incorrect Syntax on line %d, token '%s'\n", token->line, token->lexeme);
            break;
        case PA_END:
            printf("End of parsing\n");
            eot = 1;
            break;
    }
}

void parse(Token **src_tokens, int *tokensSize) {
    tokens = src_tokens;
    tokens_size = *tokensSize;
    tokens_index = 0;
    state = PS_START;
    eot = 0;

    while(!eot) {
        parse(tokens[tokens_index++]);
        printf("parsed: %s\n", tokens[tokens_index-1]->lexeme); // debugging
    }
}