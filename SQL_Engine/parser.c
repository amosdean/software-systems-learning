#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "lexer.h"


// static ParserState state;
static int eof;
static Token **tokens;
static TreeNode *rootNode;
static TreeNode *currentNode;
// TreeNode *nextNode;
static int tokens_size;
static int tokens_index;

Token *peek() {
    if (tokens_index < tokens_size) 
        return tokens[tokens_index];
    else 
        return NULL; // No more tokens
}

void consume() {
    if (tokens_index < tokens_size) 
        tokens_index++;
}

int expect(TokenType expected) {
    Token *token = peek();
    if (token->type != expected) {
        printf("Syntax Error: Expected token type %d but got %d on line %d, token '%s'\n", expected, token->type, token->line, token->lexeme);
        return 0; // Error
    }
    consume();
    return 1;
}

void parseTable(Token *token) {
    insertSibling(currentNode, token);
    token = tokens[tokens_index++];
}

void parseColumns(Token *token) {
    while(token->type == TOK_IDENTIFIER || token->type == TOK_NUMBER) {
        insertChild(currentNode, token);
        token = tokens[tokens_index++];
    }
}

void parseSelect(Token *token) {
    rootNode = createTreeNode(token);
    currentNode = rootNode;
    token = tokens[tokens_index++];
    parseColumns(token);
    parseTable(token);
}

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
}

void parseTokens(Token **src_tokens, int *tokensSize) {
    tokens = src_tokens;
    tokens_size = *tokensSize;
    tokens_index = 0;
    eof = 0;

    while(!eof) {
        parse(tokens[tokens_index++]);
        printf("parsed: %s\n", tokens[tokens_index-1]->lexeme); // debugging
    }
}
