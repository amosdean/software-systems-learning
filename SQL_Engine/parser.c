#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lexer.h"

/* Token navigation functions */
static Token *peek(Parser *parser) {
    return parser->currentToken;
}

static Token *pop(Parser *parser) {
    Token *token = parser->currentToken;
    if (!token) {
        parser->eot = 1;
        return NULL;
    }

    parser->tokensIndex++;
    parser->currentToken =
        parser->tokensIndex < parser->tokensSize
            ? parser->tokens[parser->tokensIndex]
            : NULL;

    if (!parser->currentToken) parser->eot = 1;
    printf("Popped token: type=%d, lexeme='%s', line=%d\n", token->type, token->lexeme, token->line); // Debugging
    return token;
}

static int expect(Parser *parser, TokenType expected) {
    Token *token = peek(parser);
    if (!token || token->type != expected) {
        if (token)
            printf("Syntax Error: Expected token type %d but got %d on line %d, token '%s'\n",
                   expected, token->type, token->line, token->lexeme);
        else
            printf("Syntax Error: Expected token type %d but reached end of input\n",
                   expected);
        return 0;
    }

    return 1;
}

static int expectPunctuation(Parser *parser, const char *expected) {
    Token *token = peek(parser);
    if (!token || token->type != TOK_OTHER || strcmp(token->lexeme, expected) != 0) {
        // printf("Syntax Error: Expected punctuation '%s' but got '%s' on line %d\n",
        //           expected, token->lexeme, token->line);
        return 0;
    }
    return 1;
}

/* TreeNode functions */
TreeNode * createTreeNode(Token *token) {
    TreeNode *node = calloc(1, sizeof(TreeNode));
    node->token = token;
    node->firstChild = NULL;
    node->nextSiblingNode = NULL;
    return node;
}

void insertSibling(Parser *parser) {
    TreeNode *nextNode = createTreeNode(pop(parser));
    parser->currentNode->nextSiblingNode = nextNode;
    parser->currentNode = nextNode;
}

void insertChild(Parser *parser) {
    while(parser->currentNode->firstChild != NULL)
        parser->currentNode = parser->currentNode->firstChild;
    TreeNode *nextNode = createTreeNode(peek(parser));
    parser->currentNode->firstChild = nextNode;
}

/* Parsing functions */
static int parseSemicolon(Parser *parser) {
    if (expect(parser, TOK_SEMICOLON) == 0) return 0;
    pop(parser); // Consume the semicolon.
    parser->eos = 1; // Mark end of statement after semicolon.
    return 1;
}

static int parseColumns(Parser *parser) {
    Token *token = peek(parser);

    while (token &&
           (token->type == TOK_IDENTIFIER || token->type == TOK_NUMBER)) {
        insertChild(parser);
        pop(parser);
        token = peek(parser);
    }

    return token != NULL;
}

static int parseFrom(Parser *parser) {
    // Token *from_token = peek(parser);

    // parse FROM keyword
    if (!expect(parser, TOK_FROM)) return 0;
    insertSibling(parser); // keywords are siblings of the root node

    // parse table name
    if (!expect(parser, TOK_IDENTIFIER)) return 0; 
    insertChild(parser);// child of the keyword node
    pop(parser); // Consume the table name.
    return 1;
}

static int parseSelect(Parser *parser) {
    Token *select_token = pop(parser);

    parser->currentStatement->rootNode = createTreeNode(select_token);
    if (!parser->currentStatement->rootNode) return 0;
    parser->currentNode = parser->currentStatement->rootNode;

    // pop(parser); // Consume SELECT.

    return (!parseColumns(parser) ||
        !parseFrom(parser) ||
        !parseSemicolon(parser)) 
        ? 0 : 1;
}

static int createTable(Parser *parser) {
    Token *create_token = pop(parser);

    parser->currentStatement->rootNode = createTreeNode(create_token);
    if (!parser->currentStatement->rootNode) return 0;
    parser->currentNode = parser->currentStatement->rootNode;

    if (!expect(parser, TOK_TABLE)) return 0; // Expect table name
    pop(parser); // Consume TABLE keyword
    insertChild(parser); // Add table name as child of CREATE
    pop(parser); // Consume table name

    Token *token = peek(parser);
    if (!expectPunctuation(parser, "(")) {
          printf("Syntax Error: Expected '(' after table name on line %d\n", token->line);
        return 0; 
    }
    insertSibling(parser); // Add '(' as child of CREATE and consume
    while (token && strcmp(token->lexeme, ")") != 0) {
        if (!expect(parser, TOK_IDENTIFIER)) return 0; // Expect column name
        insertChild(parser); // Add column name as child of CREATE
        pop(parser); // Consume column name
        token = peek(parser);
        if (!expect(parser, TOK_DATATYPE)) return 0; // Expect datatype
        insertChild(parser); // Add datatype as child of CREATE
        pop(parser); // Consume datatype
        token = peek(parser);
        if (token && token->lexeme && strcmp(token->lexeme, ",") == 0) {
            pop(parser); // Consume ','
            // insertSibling(parser); // Add ',' as sibling of last column
        }
        token = peek(parser);
        }
    if (!expect(parser, TOK_OTHER)) return 0; // Expect ')'
    pop(parser); // Consume ')'
    // Expect semicolon
    if (!expect(parser, TOK_SEMICOLON)) return 0;
    pop(parser); // Consume semicolon
        return 1;
}

static int parseInsert(Parser *parser) {
    Token *insert_token = pop(parser);

    // Set up root node
    parser->currentStatement->rootNode = createTreeNode(insert_token);
    if (!parser->currentStatement->rootNode) return 0;
    parser->currentNode = parser->currentStatement->rootNode;

    // parse INTO keyword
    if (!expect(parser, TOK_INTO)) return 0;
    insertSibling(parser); // Add INTO as sibling of INSERT
    // pop(parser); // Consume INTO

    // parse table name
    if (!expect(parser, TOK_IDENTIFIER)) return 0;
    insertChild(parser); // Add table name as child of INSERT
    pop(parser); // Consume table name

    // Expect '(' for column list
    if(!expectPunctuation(parser, "(")) {
        printf("Syntax Error: Expected '(' after table name on line %d\n", parser->currentToken->line);
        return 0;
    }
    insertChild(parser); // Add '(' as child of INSERT
    pop(parser); // Consume '('
    while (!expectPunctuation(parser, ")")) {
        if (!expect(parser, TOK_IDENTIFIER)) return 0; // Expect column name
        insertChild(parser); // Add column name as child of INSERT
        pop(parser); // Consume column name
            if (expectPunctuation(parser, ","))  // commas optional currently
                pop(parser); // Consume ','
                // insertSibling(parser); // Add ',' as sibling of last column
        }
    pop(parser); // Consume ')'

     // Expect VALUES keyword
    if (!expect(parser, TOK_VALUES)) return 0;
    insertSibling(parser); // Add VALUES as sibling of INSERT
    // pop(parser); // Consume VALUES

    // if (token && token->type == TOK_OTHER && strcmp(token->lexeme, "(") == 0) {
    if(!expectPunctuation(parser, "(")) {
        printf("Syntax Error: Expected '(' after VALUES on line %d\n", parser->currentToken->line);
        return 0;
    }

    // accept multiple rows
    while(expectPunctuation(parser, "(")) {
        pop(parser); // Consume '('
        // insertChild(parser); // Add '(' as child of VALUES
        while (!expectPunctuation(parser, ")")) {
            // if (!expect(parser, TOK_IDENTIFIER) && !expect(parser, TOK_NUMBER)) return 0; // Expect value
            if (parser->currentToken->type != TOK_IDENTIFIER && parser->currentToken->type != TOK_NUMBER) {
                printf("Syntax Error: Expected value (identifier or number) on line %d\n", parser->currentToken->line);
                return 0;
            }
            insertChild(parser); // Add value as child of VALUES
            pop(parser); // Consume value
            if (expectPunctuation(parser, ",")) { // commas optional currently
                pop(parser); // Consume ','
                // insertSibling(parser); // Add ',' as sibling of last value
            }
        }
        // if (!expect(parser, TOK_OTHER)) return 0; // Expect ')'
        pop(parser); // Consume ')'
        if(expectPunctuation(parser, ",")) 
            pop(parser); // Consume ','
    }
    // Expect semicolon
    if (!expect(parser, TOK_SEMICOLON)) return 0;
    pop(parser); // Consume semicolon
    return 1;
}

static int addStatement(Parser *parser, StatementType type) {
    Statement *stmt =  parser->currentStatement;
    stmt->type = type;
    parser->statements[parser->statementIndex++] = stmt;
    // stmt->rootNode = parser->
    // append to statements linked list 
    // if (!parser->rootStatement) 
    //     parser->rootStatement = stmt;
    // else {
    //     Statement *current = parser->rootStatement;
    //     while (current->next)
    //         current = current->next;
    //     current->next = stmt;
    // }
    return 1;
}

static int parseStatement(Parser *parser) {
    Token *start_token = peek(parser);
    if (!start_token) return 0;
    parser->currentStatement = malloc(sizeof(*parser->currentStatement));


    switch (start_token->type) {
        case TOK_SELECT:
            if (!parseSelect(parser)) return 0;
            if (!addStatement(parser, ST_SELECT)) return 0;
            return 1;
        case TOK_CREATE:
            // Implement parseCreate function
            if (!createTable(parser)) return 0;
            if (!addStatement(parser, ST_CREATE)) return 0;
            return 1; 
        case TOK_INSERT:
            if (!parseInsert(parser)) return 0;
            if (!addStatement(parser, ST_INSERT)) return 0;
            return 1;
        case TOK_EOF:
            return 0; // End of tokens, no more statements to parse
        default:
            printf("Syntax Error: Unexpected token '%s' on line %d\n",
                   start_token->lexeme, start_token->line);
            return 0;
    }
}

Parser *parse(Token **src_tokens, int *tokensSize) {
    if (!src_tokens || !tokensSize || *tokensSize <= 0) return 0;

    Parser *parser = calloc(1, sizeof(Parser));
    parser->tokens = src_tokens;
    parser->tokensSize = *tokensSize;
    parser->currentToken = src_tokens[0];
    parser->statements = calloc(100, sizeof(Statement *));
    parser->statementIndex = 0;

    while (parser->currentToken && !parser->eot) {
        if (!parseStatement(parser))
            break;
    }
    return parser;
}
