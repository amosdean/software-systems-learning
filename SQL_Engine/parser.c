#include <stdio.h>
#include "parser.h"
#include "lexer.h"
const ParserAction TRANSITIONS[3][3] = {
        //       SELECT      IDENTIFIER     FROM            EOF
/* START */    {TO_SELECT,   PARSER_ERR,  PARSER_ERR,   PARSER_END},
/* SELECT */   {PARSER_ERR,  PARSER_CONT, TO_FROM,      PARSER_END},
/* FROM */     {PARSER_ERR,  PARSER_CONT, PARSER_ERR,   PARSER_END}
};


ParserState state;
int eof;
Token *tokens;
TreeNode *root;
TreeNode *currentNode;
// TreeNode *nextNode;
int tokens_size;
int tokens_index;

TreeNode * createTreeNode(Token *token) {
    TreeNode *node = calloc(1, sizeof(TreeNode));
    node->token = token;
    node->firstChild = NULL;
    node->nextSiblingNode = NULL;
    return node;
}

void insertSibling(TreeNode *node, Token * token) {
    TreeNode *nextNode = createTreeNode(token);
    currentNode->nextSiblingNode = nextNode;
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
void Parse(Token *token) {
    ParserAction action = TRANSITIONS[state][token->type];
    switch(action) {
        case TO_SELECT:
            state = IN_SELECT;
            root = createTreeNode(token);
            break;
        case TO_FROM:
            state = TO_FROM;
            insertSibling(currentNode, token);
            break;
        case TO_WHERE:
            state = TO_WHERE;
            insertSibling(currentNode, token);
            break;
        case PARSER_CONT: 
            insertChild(currentNode, token);
            break;
        case PARSER_ERR: 
            printf("ERROR: Incorrect Syntax on line %d, token '%s'", token->line, token->lexeme);
            break;
        case PARSER_END:
            printf("End of parsing\n");
            eof = 1;
            break;
    }
}

void ParseTokens(Token **src_tokens, int tokensSize) {
    tokens = src_tokens;
    tokens_size = tokensSize;
    tokens_index = 0;
    state = START;
    eof = 0;
    while(!eof) {
        parse(tokens[tokens_index++]);
    }
}